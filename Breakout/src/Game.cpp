#include "Game.h"

#include <GLFW/glfw3.h>
#include <ResourceManager.h>
#include <SpriteRenderer.h>
#include <ParticleGenerator.h>
#include <PostProcesser.h>


BallObject* Ball;
GameObject* Player;
SpriteRenderer* Renderer;
ParticleGenerator* Particles;
PostProcessor* Effects;
float ShakeTime = 0.0f;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{

}

void Game::Init()
{
    // 着色器
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.fs", nullptr, "sprite");//frag->fs
    ResourceManager::LoadShader("shaders/particle.vs", "shaders/particle.fs", nullptr, "particle");
    ResourceManager::LoadShader("shaders/post_processing.vs", "shaders/post_processing.fs", nullptr, "postprocessing");

    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

    Shader& spriteShader = ResourceManager::Shaders[("sprite")];
    spriteShader.Use().SetInteger("sprite", 0);
    spriteShader.SetMatrix4("projection", projection);

    ResourceManager::GetShader("particle").Use();//use 后  shader 设置才能生效
    //ResourceManager::GetShader("particle").SetInteger("image", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

    // 渲染精灵
    Renderer = new SpriteRenderer(spriteShader);
    Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);

    // 纹理
    ResourceManager::LoadTexture("textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("textures/awesomeface.png", true, "face");
    ResourceManager::LoadTexture("textures/block.png", false, "block");
    ResourceManager::LoadTexture("textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("textures/paddle.png", true, "paddle");
    ResourceManager::LoadTexture("textures/particle.png", true, "particle");

    // 关卡
    GameLevel one; one.Load("levels/one.lvl", this->Width, this->Height / 2);
    GameLevel two; two.Load("levels/two.lvl", this->Width, this->Height / 2);
    GameLevel three; three.Load("levels/three.lvl", this->Width, this->Height / 2);
    GameLevel four; four.Load("levels/four.lvl", this->Width, this->Height / 2);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;

    //玩家
    glm::vec2 playerPos = glm::vec2(
        this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
        this->Height - PLAYER_SIZE.y
    );
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

    //球
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS,
        -BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
        ResourceManager::GetTexture("face"));

    //方形粒子效果
    Particles = new ParticleGenerator(
        ResourceManager::GetShader("particle"),
        ResourceManager::GetTexture("particle"),
        500
    );
}

//更新GameObject 位置、生命状态、运行速度、颜色等
void Game::Update(float dt)
{
    Ball->Move(dt, this->Width);

    // update particles
    Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));

    //更新shakeTime值
    if (ShakeTime > 0.0f)
    {
        ShakeTime -= dt;
        if (ShakeTime <= 0.0f)
            Effects->Shake = false;
    }
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
		Effects->BeginRender();

		// draw background
		Texture2D backgroundTexture = ResourceManager::GetTexture("background");
		Renderer->DrawSprite(backgroundTexture,
			glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f
		);
		// draw levels
		this->Levels[this->Level].Draw(*Renderer);

		//draw player
		Player->Draw(*Renderer);
		// draw particles	
		Particles->Draw();//在渲染球之前渲染粒子。这样，粒子最终会渲染在所有其他对象的前面，但在球的后面
		//draw ball
		Ball->Draw(*Renderer);

		Effects->EndRender();
		Effects->Render(glfwGetTime());

		//碰撞检测
		this->DoCollisions();
	}
}

//方形碰撞检测
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

//圆形碰撞检测
//float clamp(float value, float min, float max) {
//    return std::max(min, std::min(max, value));
//}

bool CheckCollision_round(BallObject& one, GameObject& two) // AABB - Circle collision
{
    // 获取球圆心
    glm::vec2 center(one.Position + one.Radius);
    // 计算GameObject 中心
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(
        two.Position.x + aabb_half_extents.x,
        two.Position.y + aabb_half_extents.y
    );
    // 获取两向量中心差
    glm::vec2 difference = center - aabb_center;//可以理解为两个中心的连线
    //找到方框中心与经过边框的向量
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);//约定范围-aabb_half_extents, aabb_half_extents
    // 获取邻近点的位置
    glm::vec2 closest = aabb_center + clamped;
    // 获取点位置与圆球中心距离
    difference = closest - center;
    return glm::length(difference) < one.Radius;
}


enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

typedef std::tuple<bool, Direction, glm::vec2> Collision;

Collision CheckCollision_round2(BallObject& one, GameObject& two) // AABB - Circle collision
{
    // 获取球圆心
    glm::vec2 center(one.Position + one.Radius);
    // 计算GameObject 中心
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(
        two.Position.x + aabb_half_extents.x,
        two.Position.y + aabb_half_extents.y
    );
    // 获取两向量中心差
    glm::vec2 difference = center - aabb_center;//可以理解为两个中心的连线
    //找到方框中心与经过边框的向量
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);//约定范围-aabb_half_extents, aabb_half_extents
    // 获取邻近点的位置
    glm::vec2 closest = aabb_center + clamped;
    // 获取点位置与圆球中心距离
    difference = closest - center;

    if (glm::length(difference) <= one.Radius)
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

void Game::DoCollisions()
{
    for (GameObject& box : this->Levels[this->Level].Bricks)
    {
        if (!box.Destroyed)
		{
			Collision collision = CheckCollision_round2(*Ball, box);
            if (std::get<0>(collision)) // 是否碰撞
            {
                // 非固体就销毁
                if (!box.IsSolid)
					box.Destroyed = true;
				else {
					// if block is solid, enable shake effect
					ShakeTime = 0.05f;
					Effects->Shake = true;
				}

                // 碰撞结果
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);

                //横向 左右碰撞
                if (dir == LEFT || dir == RIGHT)
                {
                    Ball->Velocity.x = -Ball->Velocity.x; // 翻转横向速度
                    // relocate
                    float penetration = Ball->Radius - std::abs(diff_vector.x);
                    if (dir == LEFT)
                        Ball->Position.x += penetration; // 球向右
                    else
                        Ball->Position.x -= penetration; // 球向左
                }
                //纵向 上下碰撞
                else 
                {
                    Ball->Velocity.y = -Ball->Velocity.y; // 翻转纵向速度
                    // relocate
                    float penetration = Ball->Radius - std::abs(diff_vector.y);
                    if (dir == UP)
                        Ball->Position.y -= penetration; // 球向上
                    else
                        Ball->Position.y += penetration; // 球向下
                }
            }
        }
    }

    // 代码检测球与玩家球拍之间是否发生碰撞：
    Collision result = CheckCollision_round2(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result))
    {
        // 当检测到碰撞时，代码根据球击中球拍的位置来调整速度：
        float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
        float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        float percentage = distance / (Player->Size.x / 2.0f);

        // 根据球拍的撞击来调整球的速度
        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;

        // 为了确保在更改 x 速度后球的总速度保持一致，做了归一化处理
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity); // keep speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)
        // 调整 y 速度，以确保球在撞击球拍后向上反弹
        //Ball->Velocity.y = -Ball->Velocity.y;
        Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);//abs 强制向上

        // 意味着当碰撞发生时，球将粘附于球拍，这样可能会改变玩法，使玩家能够控制球的释放
        //Ball->Stuck = Ball->Sticky;
        //暂时禁用音效
        //SoundEngine->play2D(FileSystem::getPath("resources/audio/bleep.wav").c_str(), false);
    }
}
