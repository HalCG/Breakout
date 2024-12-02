#include "Game.h"

#include <GLFW/glfw3.h>
#include <ResourceManager.h>
#include <SpriteRenderer.h>


BallObject* Ball;
GameObject* Player;
SpriteRenderer* Renderer;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{

}

void Game::Init()
{
    // ��ɫ��
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.fs", nullptr, "sprite");//frag->fs
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

    Shader& spriteShader = ResourceManager::Shaders[("sprite")];
    spriteShader.Use().SetInteger("sprite", 0);
    spriteShader.SetMatrix4("projection", projection);

    // ��Ⱦ����
    Renderer = new SpriteRenderer(spriteShader);

    // ����
    ResourceManager::LoadTexture("textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("textures/awesomeface.png", true, "face");
    ResourceManager::LoadTexture("textures/block.png", false, "block");
    ResourceManager::LoadTexture("textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("textures/paddle.png", true, "paddle");

    // �ؿ�
    GameLevel one; one.Load("levels/one.lvl", this->Width, this->Height / 2);
    GameLevel two; two.Load("levels/two.lvl", this->Width, this->Height / 2);
    GameLevel three; three.Load("levels/three.lvl", this->Width, this->Height / 2);
    GameLevel four; four.Load("levels/four.lvl", this->Width, this->Height / 2);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;

    //���
    glm::vec2 playerPos = glm::vec2(
        this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
        this->Height - PLAYER_SIZE.y
    );
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

    //��
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS,
        -BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
        ResourceManager::GetTexture("face"));
}

void Game::Update(float dt)
{
    Ball->Move(dt, this->Width);
}


void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        // move playerboard
        if (this->Keys[GLFW_KEY_A])
        {
            if (Player->Position.x >= 0.0f)
            {
                Player->Position.x -= velocity;
                if (Ball->Stuck)
                    Ball->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
            {
                Player->Position.x += velocity;
                if (Ball->Stuck)
                    Ball->Position.x += velocity;
            }
        }
        if (this->Keys[GLFW_KEY_SPACE])
            Ball->Stuck = false;
    }
}

void Game::Render()
{
    //Texture2D faceTexture = ResourceManager::GetTexture("face");
    //Renderer->DrawSprite(faceTexture,
    //    glm::vec2(200.0f, 200.0f), glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    if (this->State == GAME_ACTIVE)
    {
        // draw background
        Texture2D backgroundTexture = ResourceManager::GetTexture("background");
        Renderer->DrawSprite(backgroundTexture,
            glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f
        );
        // draw levels
        this->Levels[this->Level].Draw(*Renderer);
    }

    //draw player
    Player->Draw(*Renderer);

    //draw ball
    Ball->Draw(*Renderer);

    //��ײ���
    this->DoCollisions();
}

//������ײ���
bool CheckCollision_square(GameObject& one, GameObject& two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

//Բ����ײ���
//float clamp(float value, float min, float max) {
//    return std::max(min, std::min(max, value));
//}

bool CheckCollision_round(BallObject& one, GameObject& two) // AABB - Circle collision
{
    // ��ȡ��Բ��
    glm::vec2 center(one.Position + one.Radius);
    // ����GameObject ����
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(
        two.Position.x + aabb_half_extents.x,
        two.Position.y + aabb_half_extents.y
    );
    // ��ȡ���������Ĳ�
    glm::vec2 difference = center - aabb_center;//�������Ϊ�������ĵ�����
    //�ҵ����������뾭���߿������
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);//Լ����Χ-aabb_half_extents, aabb_half_extents
    // ��ȡ�ڽ����λ��
    glm::vec2 closest = aabb_center + clamped;
    // ��ȡ��λ����Բ�����ľ���
    difference = closest - center;
    return glm::length(difference) < one.Radius;
}

void Game::DoCollisions()
{
    for (GameObject& box : this->Levels[this->Level].Bricks)
    {
        if (!box.Destroyed)
        {
            if (CheckCollision_round(*Ball, box))
            {
                if (!box.IsSolid)
                    box.Destroyed = true;
            }
        }
    }
}

