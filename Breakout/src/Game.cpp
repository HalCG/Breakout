#include "game.h"

#include <ResourceManager.h>
#include <SpriteRenderer.h>

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
    // load shaders
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.fs", nullptr, "sprite");//frag->fs
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

    Shader& spriteShader = ResourceManager::Shaders[("sprite")];
    spriteShader.Use().SetInteger("sprite", 0);
    spriteShader.SetMatrix4("projection", projection);

    // set render-specific controls
    Renderer = new SpriteRenderer(spriteShader);

    // load textures
    ResourceManager::LoadTexture("textures/awesomeface.png", true, "face");
}

void Game::Update(float dt)
{

}


void Game::ProcessInput(float dt)
{

}

void Game::Render()
{
    Texture2D faceTexture = ResourceManager::GetTexture("face");
    Renderer->DrawSprite(faceTexture,
        glm::vec2(200.0f, 200.0f), glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}
