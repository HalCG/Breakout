#pragma once

#include <GLFW/glfw3.h>

// 代表了游戏的当前状态
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game
{
public:
    Game(GLuint width, GLuint height);
    ~Game();

    // 游戏状态
    GameState  State;
    GLboolean  Keys[1024];
    GLboolean  KeysProcessed[1024];
    GLuint     Width, Height;

    // 初始化游戏状态（加载所有的着色器/纹理/关卡）
    void Init();

    // 游戏循环
    void ProcessInput(GLfloat dt);
    void Update(GLfloat dt);
    void Render();
};