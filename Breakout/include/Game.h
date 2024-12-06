#ifndef GAME_H
#define GAME_H

#include "GameLevel.h"
#include "BallObject.h"
#include "PowerUp.h"

// 指定玩家的尺寸，宽度为100个单位，高度为20个单位。
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
// 定义了玩家移动的速度，设置为每秒500个单位
const float PLAYER_VELOCITY(500.0f);
// 个向量表示球的初始速度和方向，水平速度为100个单位，垂直速度为-350个单位（在许多图形上下文中，y轴是反向的，所以这个值表示向上移动）。
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// 定义了球的大小，半径为12.5个单位。
const float BALL_RADIUS = 12.5f;

// 代表了游戏的当前状态
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game
{
public:
    Game(unsigned int width, unsigned int height);
    ~Game();

    // 游戏状态
    //BallObject* Ball;
    GameState  State;
    bool  Keys[1024];
    bool  KeysProcessed[1024];
    unsigned int     Width, Height;

    std::vector<GameLevel> Levels;
    unsigned int           Level;

    std::vector<PowerUp>  PowerUps;
    void SpawnPowerUps(GameObject& block);
    void UpdatePowerUps(float dt);

    // 初始化游戏状态（加载所有的着色器/纹理/关卡）
    void Init();

    // 游戏循环
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void DoCollisions();
};

#endif

/*
通常情况下，`glad` 的头文件应该放在源文件（.c 或 .cpp 文件）中，而不是在头文件（.h 文件）中。以下是一些原因和最佳实践：

1. **避免重复包含**：如果你把 `glad` 的包含放在头文件中，而这个头文件被多个源文件包含，可能会导致重复包含问题，增加编译时间，并可能引发潜在的符号冲突。

2. **控制包含的上下文**：在源文件中包含 `glad` 允许你更好地控制每个源文件的依赖关系，尤其在每个源文件中只需要知道其自己需要的依赖，而不是全部。

3. **与其他库的集成**：通常图形库，如 OpenGL、GLFW 或 SDL，推荐在源文件中引入。这样可以保持代码的模块化和清晰，避免不必要的包含。

### 使用示例

在你的源文件中，应该这样包含 `glad`：

```c
#include <glad/glad.h> // glad must come before include OpenGL headers
#include <GLFW/glfw3.h> // GLFW (或者其他图形库)
```

确保在这之前没有显式地包含任何 OpenGL 的头文件，因为 `glad` 主要负责加载 OpenGL 函数并自带了必要的 OpenGL 定义。

### 总结

将 `glad` 的包含放在源文件中是一种良好的实践，这会帮助你避免潜在的问题，并提高项目的维护性和可读性。

*/