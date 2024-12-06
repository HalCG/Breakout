#include "PostProcesser.h"

#include <iostream>


/*
    渲染缓冲为原始的渲染帧，放入多重采样后，处理抗锯齿，然后再放入帧缓冲中，然后输出到TEXTURE0 中，再render中进行渲染
*/
PostProcessor::PostProcessor(Shader shader, unsigned int width, unsigned int height)
    : PostProcessingShader(shader), Texture(), Width(width), Height(height), Confuse(false), Chaos(false), Shake(false)
{
    //==============================================================================================================
    // 1. 初始化帧缓冲对象 
    // 生成帧缓冲：生成一个多重采样帧缓冲对象 (MSFBO)，一个常规帧缓冲对象 (FBO)，以及一个渲染缓冲对象 (RBO)。
    glGenFramebuffers(1, &this->MSFBO);
    glGenFramebuffers(1, &this->FBO);
    glGenRenderbuffers(1, &this->RBO);

    // 初始化多重采样颜色缓冲  
    // 绑定和配置 RBO：将渲染缓冲对象分配存储空间,并绑定到多重采样帧缓冲。这里使用 4 倍多重采样（抗锯齿）。
    glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);

    //为一个多重采样渲染缓冲对象（Renderbuffer Object, RBO）分配内存，并定义其存储格式。
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height); 
    // 将 RBO 附加到 MSFBO：将渲染缓冲对象附加到多重采样帧缓冲的颜色附件。
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO); 

    // 检查帧缓冲状态：检查多重采样帧缓冲是否初始化成功。
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;

    //==============================================================================================================
    // 2. 初始化常规 FBO 和纹理
    // 还初始化 FBO / 纹理以将多重采样颜色缓冲区传输至；用于着色器操作（用于后处理效果）
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    this->Texture.Generate(width, height, NULL);

    //将一个纹理（texture）附加到帧缓冲区（framebuffer）的指定附加点（attachment point）。这是现代 OpenGL 渲染管线中的关键步骤，允许你将渲染结果直接输出到纹理中，而不是默认的窗口系统（例如，显示器）
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->Texture.ID, 0); 
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;

    //解绑帧缓冲：将当前帧缓冲解绑，恢复到默认帧缓冲。
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //==============================================================================================================
    // 3. 初始化渲染数据和着色器 uniform
    //初始化渲染数据：调用 initRenderData() 函数来设置顶点数组对象（VAO）和顶点缓冲对象（VBO）。
    this->initRenderData();
    this->PostProcessingShader.SetInteger("scene", 0, true);

    //设置偏移量：创建一个偏移量数组，用于后期处理效果（如模糊）。然后将其传递给着色器
    float offset = 1.0f / 300.0f;
    float offsets[9][2] = {
        { -offset,  offset  },  // top-left
        {  0.0f,    offset  },  // top-center
        {  offset,  offset  },  // top-right
        { -offset,  0.0f    },  // center-left
        {  0.0f,    0.0f    },  // center-center
        {  offset,  0.0f    },  // center - right
        { -offset, -offset  },  // bottom-left
        {  0.0f,   -offset  },  // bottom-center
        {  offset, -offset  }   // bottom-right    
    };
    glUniform2fv(glGetUniformLocation(this->PostProcessingShader.ID, "offsets"), 9, (float*)offsets);

    //设置边缘检测内核：定义一个边缘检测内核，并将其传递给着色器。
    int edge_kernel[9] = {
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    };
    glUniform1iv(glGetUniformLocation(this->PostProcessingShader.ID, "edge_kernel"), 9, edge_kernel);

    //设置模糊内核：定义一个模糊内核，并将其传递给着色器。
    float blur_kernel[9] = {
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
        2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
    };
    glUniform1fv(glGetUniformLocation(this->PostProcessingShader.ID, "blur_kernel"), 9, blur_kernel);
}

void PostProcessor::BeginRender()
{
    //绑定多重采样帧缓冲：在渲染之前绑定 MSFBO。
    glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);

    //清除颜色缓冲：设置清除颜色为黑色，并清除颜色缓冲区，以准备渲染新的场景。
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::EndRender()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, this->MSFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);

    //将 MSFBO 中的颜色缓冲区解析到 FBO 中，以便后续的后期处理。
    glBlitFramebuffer(0, 0, this->Width, this->Height, 0, 0, this->Width, this->Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // 使绑定的读写缓冲区到默认状态
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessor::Render(float time)
{
    // 使用后期处理着色器并设置时间和效果选项的 uniform。
    this->PostProcessingShader.Use();
    this->PostProcessingShader.SetFloat("time", time);
    this->PostProcessingShader.SetInteger("confuse", this->Confuse);
    this->PostProcessingShader.SetInteger("chaos", this->Chaos);
    this->PostProcessingShader.SetInteger("shake", this->Shake);

    // 绑定纹理和 VAO，然后绘制一个覆盖整个屏幕的四边形，以显示后期处理效果。
    glActiveTexture(GL_TEXTURE0);// 激活纹理单元 0
    this->Texture.Bind();
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void PostProcessor::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = {
        // pos        // tex
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->VAO);
    glEnableVertexAttribArray(0);// 启用位置属性数组，结合shader文件看
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


/*
`glBlitFramebuffer` 函数用于在 OpenGL 中将一个帧缓冲区的内容复制到另一个帧缓冲区。这在实现图像处理或者后期效果时非常有用，比如将多重采样的缓冲区解析到单一的纹理中。让我们逐个参数分析这条命令的具体含义：

### 语法

```cpp
glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, filter);
```

### 参数

1. **源矩形参数：**
   - `srcX0` 和 `srcY0`：源矩形的左下角坐标 (0, 0)。
   - `srcX1` 和 `srcY1`：源矩形的右上角坐标 (`this->Width`, `this->Height`)。

   这部分定义了从哪个区域的帧缓冲区复制数据。在这个例子中，它表示从原点 (0, 0) 到 (Width, Height)，即整个帧缓冲的大小。

2. **目标矩形参数：**
   - `dstX0` 和 `dstY0`：目标矩形的左下角坐标 (0, 0)。
   - `dstX1` 和 `dstY1`：目标矩形的右上角坐标 (`this->Width`, `this->Height`)。

   这部分定义了数据应该被复制到哪个区域。在这里，它同样表示复制数据到另一个帧缓冲的整个区域。

3. **`GL_COLOR_BUFFER_BIT`**：指定要复制的缓冲区类型。在这里，这表示我们只想复制颜色缓冲部分。`GL_COLOR_BUFFER_BIT` 是一个常量，表示将源帧缓冲的颜色信息复制到目标帧缓冲。

4. **`GL_NEAREST`**：指定过滤模式。在这个例子中，使用了最近点过滤（即最简单的方式），这意味着在进行复制时，不进行任何插值计算，直接取最近的像素值。

### 整体含义

因此，以下代码行：

```cpp
glBlitFramebuffer(0, 0, this->Width, this->Height, 0, 0, this->Width, this->Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
```

的含义是：

- 从当前绑定的读取帧缓冲区（通常是多重采样帧缓冲）中的 `(0, 0)` 到 `(this->Width, this->Height)` 区域复制颜色数据。
- 将这些数据写入到当前绑定的绘制帧缓冲区（如普通的帧缓冲）中的相应区域，大小也为 `(0, 0)` 到 `(this->Width, this->Height)`。
- 采用最近点颜色插值方式进行复制。

### 应用场景

这种操作通常在进行后期处理效果时使用，例如在使用多重采样抗锯齿后，将高质量的颜色缓冲区解析（blit）到一个单一的纹理中，以便后续的着色器效果（如模糊、边缘检测等）。通过这种方式，你可以在减少抗锯齿的同时优化渲染效果。

*/

/*
`glRenderbufferStorageMultisample` 函数用于在 OpenGL 中为多重采样渲染缓冲对象（renderbuffer）分配存储。它专门用于创建具有多重采样（anti-aliasing）能力的渲染缓冲，以提高渲染质量，减少锯齿现象。

### 函数原型

```cpp
void glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
```

### 参数详解

1. **`target`**：
   - 类型：`GLenum`
   - 说明：指定渲染缓冲的类型。在这里，我们使用 `GL_RENDERBUFFER` 表示我们正在操作一个渲染缓冲对象。

2. **`samples`**：
   - 类型：`GLsizei`
   - 说明：指定要使用的多重采样的数量。一个典型的值是 `4`，表示每个像素使用 4 次采样。更多的样本会提供更好的抗锯齿效果，但也会增加内存使用和计算成本。

3. **`internalformat`**：
   - 类型：`GLenum`
   - 说明：指定渲染缓冲的内部格式。在这个例子中，使用 `GL_RGB`，意味着存储的颜色格式为 RGB（红绿蓝）。这决定了颜色的存储格式和占用的内存。

4. **`width`** 和 **`height`**：
   - 类型：`GLsizei`
   - 说明：指定渲染缓冲的宽度和高度，以像素为单位。这定义了渲染缓冲的尺寸。

### 整体含义

`glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height);` 此行代码的作用是：

- **创建一个新的多重采样渲染缓冲对象**，并为其分配内存。
- **设置该渲染缓冲的参数**：将样本数设置为 `4`（表示启用 4x 多重采样），内部格式为 `GL_RGB`，宽度和高度分别为 `width` 和 `height`。

这个函数通常在创建多重采样帧缓冲对象时使用，配合 `glFramebufferRenderbuffer` 将其附加到帧缓冲中。

### 应用场景

- **抗锯齿**：通过使用多重采样，渲染出的图像将在边缘处减少锯齿化的现象，从而提高图形质量。
- **调试绘制**：渲染缓冲可以在离屏渲染中使用，将渲染成果输出到纹理，然后进行后期处理效果。

使用 `glRenderbufferStorageMultisample` 是进行高质量图像渲染的重要步骤，尤其在需要抗锯齿的应用场景（如游戏、图形应用等）中非常频繁。


`glRenderbufferStorageMultisample` 函数用于创建多重采样（multisampling）渲染缓冲区。这种技术用于提高图形渲染的质量，特别是在抗锯齿方面。多重采样在渲染过程中可以减少锯齿边缘，使得图形的边缘更加平滑。

### 工作原理:
1. **多重采样的概念**：
   - 在渲染时，每个像素的颜色值并不是通过一个单一的采样点来确定，而是通过多个评估点（或子采样点）来进行采样。这些采样点分别在一个像素内，并且每个采样点将得到一个单独的颜色值。

2. **缓冲区存储**：
   - `glRenderbufferStorageMultisample` 创建的渲染缓冲区会为每个像素分配多个样本（在你的示例中是 4 个样本，即 4x MSAA 效果）。这些样本分别存储在渲染缓冲区中，并在最终合成颜色值时会计算它们的平均值或者采用其他合成算法。

3. **类型的采样**:
   - 在你的示例中使用了 `GL_RGB`，这意味着每个样本存储 RGB 颜色信息。具体存储的格式取决于 OpenGL 的具体实现和配置，但基本上它会为每个样本存储三种颜色通道的信息。

### 总结:
简而言之，调用 `glRenderbufferStorageMultisample` 会创建一个具有多重采样能力的渲染缓冲区，每个像素具有多个样本点，这样在执行抗锯齿时就能更好地平滑边缘，提升图形质量。如果你想更深入了解多重采样的具体算法或者实现细节，可以进一步讨论！


*/

/*
让我们逐行解析这两个 OpenGL 函数调用，理解它们的作用和如何一起工作。

### 1. `glRenderbufferStorageMultisample`

```cpp
glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height);
```

- **函数目的**：为一个多重采样渲染缓冲对象（Renderbuffer Object, RBO）分配内存，并定义其存储格式。

- **参数解析**：
  - **`GL_RENDERBUFFER`**：这是目标类型，表示我们正在创建一个渲染缓冲对象。
  - **`4`**：表示该渲染缓冲使用 4x 多重采样（每个像素采样 4 次）。这有助于减少锯齿现象，提升渲染质量。
  - **`GL_RGB`**：指定存储的内部格式为 RGB（即每个像素由红、绿、蓝三种颜色组成），说明了数据的颜色格式。
  - **`width` 和 `height`**：指定渲染缓冲的宽度和高度（以像素为单位），定义了渲染缓冲的尺寸。

### 2. `glFramebufferRenderbuffer`

```cpp
glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO);
```

- **函数目的**：将之前创建的多重采样渲染缓冲对象附加到当前的帧缓冲对象（Framebuffer Object, FBO）上，以便在渲染过程中使用。

- **参数解析**：
  - **`GL_FRAMEBUFFER`**：这是目标类型，表示我们正在操作一个帧缓冲对象。
  - **`GL_COLOR_ATTACHMENT0`**：这是附加的目标，指向颜色附件 0。OpenGL 允许用户附加多个附件，但通常第一个颜色附件使用 0 作为索引。
  - **`GL_RENDERBUFFER`**：再次指定我们附加的是一个渲染缓冲而不是纹理。
  - **`this->RBO`**：这是之前创建的渲染缓冲对象的 ID，表示我们希望将这个特定的渲染缓冲附加到帧缓冲中。

### 总体功能和流程

这两行代码的总体功能可以总结如下：

1. **创建多重采样的渲染缓冲**：第一行代码使用 `glRenderbufferStorageMultisample` 为一个多重采样的渲染缓冲对象分配内存，并指定了多重采样和格式。这意味着在后续的渲染过程中，每个像素都将会有多个样本，最大程度地减少锯齿现象。

2. **将渲染缓冲附加到帧缓冲**：第二行代码将这个渲染缓冲对象附加到当前的帧缓冲的颜色附件中，以便后续的渲染操作可以将结果输出到这个缓冲中。

### 应用场景

这种设置通常用于需要高质量渲染的场景，例如游戏或图形应用。在启用多重采样后，图形渲染的输出将显著改善，从而在视觉上更加平滑且没有明显的锯齿边缘。

### 注意事项

- 在使用多重采样时需要确保支持该功能的 OpenGL 上下文（通常是提供较新版本的 OpenGL，支持多重采样）。
- 在完成渲染后，可能会需要使用 `glBlitFramebuffer` 等函数将多重采样的渲染结果解析到其他纹理或帧缓冲中，以便进行后期处理或后续渲染。

*/

/*
`glFramebufferTexture2D` 是 OpenGL 的一个函数，用于将一个纹理（texture）附加到帧缓冲区（framebuffer）的指定附加点（attachment point）。在你的例子中，纹理被附加为颜色附加点（color attachment）。这是现代 OpenGL 渲染管线中的关键步骤，允许你将渲染结果直接输出到纹理中，而不是默认的窗口系统（例如，显示器）。以下是这个函数的详细说明和用途：

### 作用：
1. **附加纹理到帧缓冲区**：
   - `glFramebufferTexture2D` 将指定的纹理（在你的例子中是 `this->Texture.ID`）附加到当前的帧缓冲区上，使其成为该缓冲区的一个颜色附加点。`GL_COLOR_ATTACHMENT0` 表示这是第一个颜色附加点。

2. **渲染到纹理**：
   - 通过将纹理附加到帧缓冲区，所有绘制到这个帧缓冲区的渲染操作都将最终输出到这个纹理中，而不是屏幕。这使得纹理可用于后续的渲染操作，比如处理后效（post-processing）、纹理映射（texture mapping）或作为其他物体的贴图（texture）。

3. **多种渲染效果**：
   - 使用帧缓冲区的一个主要目的是实现复杂的渲染效果。例如，可以将场景渲染到一个纹理上，然后将该纹理应用于屏幕上的四边形（quad）以进行后效处理。此外，附加多个颜色附加点可以实现多重渲染输出，以便在多个纹理上渲染不同的信息（如颜色、深度、法线等）。

### 参数详解：
- `GL_FRAMEBUFFER`：当前绑定的帧缓冲区对象。
- `GL_COLOR_ATTACHMENT0`：要附加的颜色附加点，OpenGL 支持多个颜色附加点（例如 `GL_COLOR_ATTACHMENT1`、`GL_COLOR_ATTACHMENT2` 等）。
- `GL_TEXTURE_2D`：要附加的纹理的目标类型，表明这是一种二维纹理。
- `this->Texture.ID`：要附加的纹理的句柄或 ID。这通常是在纹理生成和配置时获得的。
- `0`：这个参数表示纹理的 mipmap 层级，通常对于大多数用例，使用 0 表示使用基础级（base level）。

### 过程：
1. **初始化纹理**：在使用 `glFramebufferTexture2D` 之前，需要确保纹理被正确创建和配置，例如设置纹理的格式和尺寸等。
2. **绑定帧缓冲区**：调用此函数之前，需确保帧缓冲区对象已经被绑定（使用 `glBindFramebuffer`）。
3. **检查状态**：附加完纹理后，可以通过 `glCheckFramebufferStatus` 检查帧缓冲区的状态，确保其有效。

### 实际应用示例：
假设你正在开发一个游戏，并希望实现屏幕模糊效果。你可以首先将当前帧渲染到一个纹理，然后使用该纹理进行模糊处理，最后将处理后的纹理显示到屏幕上。

总之，`glFramebufferTexture2D` 是现代 OpenGL 渲染管线中的一个基本构建块，使得高效的渲染和后处理成为可能。如果你有更多具体问题或者想要深入探讨某个方面，请告诉我！

*/

/*
在这段代码中，`offset` 是通过将 1.0f 除以 300.0f 来计算的，结果是每个偏移量的幅度。选择将 1.0f 除以 300.0f 的原因通常与图形应用中的具体需求、场景大小或视觉效果有关。

**可能的原因包括：**

1. **分辨率相关性**：在一些图形程序中，尤其是在2D渲染中，常常会依赖于屏幕分辨率进行计算。将 1.0f 除以 300.0f，可以让偏移量适应特定的分辨率，以便在大屏幕和小屏幕上保持一致的视觉效果。

2. **控制精度**：300是一个相对较大的数值，会导致offset的值相对较小（约为0.0033）。这使得偏移量在渲染、运动或动画时更加精细，避免了视觉上的突兀感。

3. **场景大小**：如果应用程序中有特定的逻辑或场景的比例，例如在某些游戏或应用中，需要确保偏移量与背景或对象的实际尺寸匹配，可能就会使用这样的常数。

4. **视觉效果**：具体的值选择可能出于视觉效果的考虑。开发者通过实验得出最佳效果时使用的具体值。

当然，选择这个特定值的真正原因可能与具体应用的上下文和开发者的设计决策相关。如果有更多的上下文信息或代码功能说明，可以提供更详细的分析。

*/