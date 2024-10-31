#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game.h"
#include "ResourceManager.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

Game Breakout(SCREEN_WIDTH, SCREEN_HEIGHT);

int main(int argc, char* argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // OpenGL configuration
    // --------------------
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // initialize game
    // ---------------
    Breakout.Init();

    // deltaTime variables
    // -------------------
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        // calculate delta time
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();

        // manage user input
        // -----------------
        Breakout.ProcessInput(deltaTime);

        // update game state
        // -----------------
        Breakout.Update(deltaTime);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        Breakout.Render();

        glfwSwapBuffers(window);
    }

    // delete all resources as loaded using the resource manager
    // ---------------------------------------------------------
    ResourceManager::Clear();

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // when a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            Breakout.Keys[key] = true;
        else if (action == GLFW_RELEASE)
        {
            Breakout.Keys[key] = false;
            Breakout.KeysProcessed[key] = false;
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


/*
这段代码是设置 OpenGL 的视口并启用混合以处理透明度的一部分。下面是对每一部分的详细解释：

### 代码解析

1. **设置视口**:
   ```cpp
   glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
   ```
   - `glViewport` 函数设置视口的大小和位置。视口定义了窗口中显示 OpenGL 渲染结果的区域。
   - 参数说明：
     - `0`, `0`: 视口的左下角在窗口中的坐标（以像素为单位）。
     - `SCREEN_WIDTH`, `SCREEN_HEIGHT`: 视口的宽度和高度，这里通常代表窗口的宽度和高度。
   - 此调用确保 OpenGL 渲染结果覆盖整个窗口。

2. **启用混合**:
   ```cpp
   glEnable(GL_BLEND);
   ```
   - `glEnable` 函数启用某个 OpenGL 功能，这里启用的是混合（alpha blending）功能。混合能够根据源和目标颜色的透明度值组合颜色，从而实现透明效果。

3. **设置混合函数**:
   ```cpp
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   ```
   - `glBlendFunc` 函数设置混合函数，即源颜色（即正在绘制的颜色）和目标颜色（即背景颜色）的混合方式。
   - 参数说明：
     - `GL_SRC_ALPHA`: 表示用来决定源颜色影响的因子，这里表示使用源颜色的 alpha 值（透明度）。
     - `GL_ONE_MINUS_SRC_ALPHA`: 表示用来决定目标颜色影响的因子，这里表示使用 1 减去源颜色的 alpha 值。
   - 这种设置将使得在源颜色混合时依据其透明度来与背景颜色进行组合，从而实现常见的半透明效果。

### 总结

这段代码的主要目的是设置 OpenGL 的渲染状态，以便在应用程序中正确处理视口和透明度。通过设置视口，确保渲染内容覆盖整个窗口；通过启用混合和设置适当的混合函数，可以在图形中实现平滑的透明效果。这在许多视觉效果（如精灵、过渡效果等）中显得尤为重要。

*/