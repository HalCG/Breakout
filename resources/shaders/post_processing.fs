#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D scene;
uniform vec2  offsets[9];
uniform int     edge_kernel[9];
uniform float  blur_kernel[9];

uniform bool chaos;
uniform bool confuse;
uniform bool shake;

void main()
{
    //在开始处理之前，确保颜色初始化为黑色（透明）。
    color = vec4(0.0f);

    vec3 sample[9];
    // 只有在启用混乱或震动效果时，才从纹理中进行多次采样。每个采样使用偏移量来获取相邻像素的颜色。
    if(chaos || shake)
        for(int i = 0; i < 9; i++)
            sample[i] = vec3(texture(scene, TexCoords.st + offsets[i]));

    // 混乱效果:使用边缘卷积核对采样的颜色进行加和处理，计算边缘效果。
    if(chaos)
    {
        for(int i = 0; i < 9; i++)
            color += vec4(sample[i] * edge_kernel[i], 0.0f);
        color.a = 1.0f;
    }
    //直接获取当前纹理上的颜色，并且将其进行反转处理。
    else if(confuse)
    {
        color = vec4(1.0 - texture(scene, TexCoords).rgb, 1.0);
    }
    //使用模糊卷积核对采样的颜色进行加和，计算最终的模糊效果。
    else if(shake)
    {
        for(int i = 0; i < 9; i++)
            color += vec4(sample[i] * blur_kernel[i], 0.0f);
        color.a = 1.0f;
    }
    else
    {
        color =  texture(scene, TexCoords);
    }
}

/*
这段片段着色器代码是用 GLSL (OpenGL Shading Language) 编写的，适用于 OpenGL 版本 3.3 及以上。它处理纹理操作和特效，主要用于创建一些视觉效果，比如图像模糊、翻转颜色以及创建“混乱”和“震动”的效果。下面是对每个部分的详细解释：

### 代码详解

1. **输入和输出**:
   ```glsl
   in vec2 TexCoords;  // 输入的纹理坐标
   out vec4 color;     // 输出的颜色值
   ```
   - `TexCoords`: 传入的纹理坐标，通常由顶点着色器生成，指定了在纹理中该片段的采样位置。
   - `color`: 该片段的输出颜色值。

2. **统一变量 (Uniform Variables)**:
   ```glsl
   uniform sampler2D scene;       // 用于采样的纹理
   uniform vec2  offsets[9];      // 存储用于纹理采样的偏移量
   uniform int     edge_kernel[9]; // 边缘检测卷积核
   uniform float  blur_kernel[9];  // 模糊卷积核
   ```
   - `scene`: 这是一个2D纹理，通常是在主程序中通过 OpenGL ID 传入的。它是用于渲染的材质或图像，比如场景的颜色缓冲。
   - `offsets`: 用于在处理效果时指定相对于纹理坐标的偏移量数组（默认为 3x3 矩阵的九个偏移）。
   - `edge_kernel` 和 `blur_kernel`: 用于边缘检测和模糊处理的卷积核数组。

3. **效果开关 (Effect Switches)**:
   ```glsl
   uniform bool chaos;   // 混乱效果开关
   uniform bool confuse;  // 颜色混淆开关
   uniform bool shake;    // 震动效果开关
   ```
   - 这三个布尔变量用于控制不同的效果，只有在对应的效果被打开时，相关的处理代码才会执行。

4. **主函数 (main function)**:
   - **初始化 `color`**:
     ```glsl
     color = vec4(0.0f);
     ```
     - 在开始处理之前，确保颜色初始化为黑色（透明）。

   - **采样纹理**:
     ```glsl
     vec3 sample[9];
     if(chaos || shake)
         for(int i = 0; i < 9; i++)
             sample[i] = vec3(texture(scene, TexCoords.st + offsets[i]));
     ```
     - 只有在启用混乱或震动效果时，才从纹理中进行多次采样。每个采样使用偏移量来获取相邻像素的颜色。

   - **混乱效果处理**:
     ```glsl
     if(chaos)
     {           
         for(int i = 0; i < 9; i++)
             color += vec4(sample[i] * edge_kernel[i], 0.0f);
         color.a = 1.0f;
     }
     ```
     - 如果启用了混乱效果，使用边缘卷积核对采样的颜色进行加和处理，计算边缘效果。

   - **颜色翻转效果**:
     ```glsl
     else if(confuse)
     {
         color = vec4(1.0 - texture(scene, TexCoords).rgb, 1.0);
     }
     ```
     - 启用翻转（混淆）效果时，直接获取当前纹理上的颜色，并且将其进行反转处理。

   - **震动效果处理**:
     ```glsl
     else if(shake)
     {
         for(int i = 0; i < 9; i++)
             color += vec4(sample[i] * blur_kernel[i], 0.0f);
         color.a = 1.0f;
     }
     ```
     - 如果启用了震动效果，使用模糊卷积核对采样的颜色进行加和，计算最终的模糊效果。

   - **默认渲染**:
     ```glsl
     else
     {
         color =  texture(scene, TexCoords);
     }
     ```
     - 如果没有启用任何特效，则正常从纹理中采样并返回颜色。

### `scene` 的来源：
- **纹理绑定**: `scene` 纹理通常是在 OpenGL 主程序部分创建和配置的，具体是在使用 `glGenTextures()`、`glBindTexture()` 和 `glTexImage2D()` 等函数时定义的。
- **传递到着色器**: 为了在片段着色器中使用 `scene`，需要通过 `glUniform1i()` 函数将纹理单元的索引传递给着色器。具体来说，通常会在绘制之前进行这样的操作：
   
  ```cpp
  glActiveTexture(GL_TEXTURE0);  // 激活纹理单元 0
  glBindTexture(GL_TEXTURE_2D, sceneTextureID);  // 绑定纹理
  glUniform1i(glGetUniformLocation(shaderProgram, "scene"), 0); // 将纹理单元索引传递到着色器
  ```

### 总结
这段代码展示了一个通过条件控制多种视觉效果的片段着色器，依赖于从主程序传入的纹理和效果开关。使用卷积核对邻域像素进行处理，从而实现了边缘检测、颜色翻转和模糊等效果。这种技术在许多图形应用中很常见，例如游戏渲染和图像处理。 如果你有更具体的问题或需要进一步的说明，请告诉我！

*/

/*
`TexCoords.st` 这段代码通常出现在图形编程或着色器编程中，并表示一个纹理坐标的结构。具体来说，`TexCoords` 是一个包含纹理坐标的变量，而 `st` 通常代表两个分量：

1. **s**：对应于纹理的水平坐标（X方向），通常表示在纹理中的列。
2. **t**：对应于纹理的垂直坐标（Y方向），通常表示在纹理中的行。

### 详细解释

- **纹理坐标**：纹理坐标是用于映射纹理到物体表面的坐标系。每个像素（纹理元，texel）的坐标值通常在 [0, 1] 范围内。例如，坐标 `(0, 0)` 表示纹理的左下角，而 `(1, 1)` 表示右上角。

- **坐标系统**：在很多图形系统中，`s` 和 `t` 坐标是相对于纹理图像的；如同在二维平面上的二维坐标系。

### 示例用法
在 OpenGL 或 DirectX 等图形 API 中，您可能会看到类似以下的代码：
```cpp
// 假设我们有一个结构体来存储纹理坐标
struct TexCoord {
    float s;
    float t;
};

// 创建一个纹理坐标实例
TexCoord TexCoords;
TexCoords.s = 0.5f; // 水平中心
TexCoords.t = 1.0f; // 上侧
```

### 小结
`TexCoords.st` 是对纹理采样时使用坐标的引用。理解纹理坐标对于正确渲染和处理纹理至关重要，如果你有更具体的上下文或例子，可以分享，我可以提供更详细的说明！

*/