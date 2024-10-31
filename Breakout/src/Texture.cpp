#include <iostream>

#include "texture.h"


Texture2D::Texture2D()
    : Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR)
{
    glGenTextures(1, &this->ID);
}

void Texture2D::Generate(GLuint width, GLuint height, unsigned char* data)
{
    this->Width = width;
    this->Height = height;
    // Create Texture
    glBindTexture(GL_TEXTURE_2D, this->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
    // Set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, this->ID);
}


/*
在 OpenGL 中，这些常量用于设置纹理的参数。以下是每个常量的详细说明：

### 1. `GL_TEXTURE_WRAP_S`

- **含义**: 这个参数用于指定纹理在 S 方向（通常对应于纹理坐标的 X 轴）上的包装方式。
- **常用值**:
  - `GL_REPEAT`: 重复纹理，超出范围的部分将会重复纹理。
  - `GL_CLAMP_TO_EDGE`: 将纹理坐标限制在 [0, 1] 范围内，超出范围的坐标将会采样纹理边缘的颜色。
  - `GL_MIRRORED_REPEAT`: 重复纹理，但在每个周期中翻转纹理。

### 2. `GL_TEXTURE_WRAP_T`

- **含义**: 这个参数用于指定纹理在 T 方向（通常对应于纹理坐标的 Y 轴）上的包装方式。它的用法与 `GL_TEXTURE_WRAP_S` 相似。
- **常用值**: 同 `GL_TEXTURE_WRAP_S` 参数值。

### 3. `GL_TEXTURE_MIN_FILTER`

- **含义**: 这个参数指定当纹理的真实像素尺寸小于屏幕像素尺寸时，如何选择纹理的过滤方法，影响纹理缩小时的渲染效果。
- **常用值**:
  - `GL_NEAREST`: 使用最近的纹素（点过滤）。
  - `GL_LINEAR`: 使用线性插值（线性过滤）。
  - `GL_NEAREST_MIPMAP_NEAREST`: 使用最近的 mipmap 级别和最近的纹素。
  - `GL_LINEAR_MIPMAP_NEAREST`: 使用最近的 mipmap 级别和线性过滤。
  - `GL_NEAREST_MIPMAP_LINEAR`: 使用线性插值选择 mipmap 级别，使用最近纹素的插值。
  - `GL_LINEAR_MIPMAP_LINEAR`: 使用线性插值选择 mipmap 级别，使用线性过滤。

### 4. `GL_TEXTURE_MAG_FILTER`

- **含义**: 这个参数指定当纹理的真实像素尺寸大于屏幕像素尺寸时，如何选择纹理的过滤方式，影响纹理放大时的渲染效果。
- **常用值**:
  - `GL_NEAREST`: 使用最近的纹素（点过滤）。
  - `GL_LINEAR`: 使用线性插值（线性过滤）。

这些常量通常被设置为纹理对象的参数，以控制纹理在不同情况下的显示和使用方式。通过这些设置，开发者可以获得更高质量的视觉效果，减少纹理失真和锯齿现象。

1. **生成纹理对象**:
   ```cpp
   glGenTextures(1, &this->ID);
   ```
   - `glGenTextures` 函数用于生成一个或多个纹理对象。`1` 表示生成一个纹理，`&this->ID` 是一个纹理 ID 的指针，将接收生成的纹理对象的标识符。

2. **设置纹理的宽度和高度**:
   ```cpp
   this->Width = width;
   this->Height = height;
   ```
   - 将传入的纹理宽度和高度保存到类成员变量 `Width` 和 `Height` 中，以便在后续的操作中使用。

3. **绑定纹理**:
   ```cpp
   glBindTexture(GL_TEXTURE_2D, this->ID);
   ```
   - `glBindTexture` 函数将生成的纹理对象绑定到当前的纹理单元，使后续的纹理操作都作用于这个纹理。`GL_TEXTURE_2D` 指定了纹理的类型。

4. **定义纹理图像**:
   ```cpp
   glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
   ```
   - `glTexImage2D` 函数用于创建和定义 2D 纹理。参数说明如下：
     - `GL_TEXTURE_2D`: 指定纹理的类型。
     - `0`: mipmap 级别，0 表示基色纹理。
     - `this->Internal_Format`: 纹理内部格式，决定了纹理的颜色组件（如 GL_RGB、GL_RGBA 等）。
     - `width`, `height`: 纹理的宽度和高度。
     - `0`: 边境宽度（对于 2D 纹理通常为 0）。
     - `this->Image_Format`: 用于指定提供的图像数据格式（如 GL_RGB、GL_RGBA）。
     - `GL_UNSIGNED_BYTE`: 数据类型指示符，通常对应于图像数据的类型。
     - `data`: 指向图像数据的指针。

5. **设置纹理参数**:
   ```cpp
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
   ```
   - 这里的 `glTexParameteri` 用于设置纹理的参数，以控制纹理的包装和过滤行为：
     - `GL_TEXTURE_WRAP_S` 和 `GL_TEXTURE_WRAP_T`：决定了纹理在 S 和 T 方向（即 X 和 Y 方向）的包装方式（重复、边缘夹紧等）。
     - `GL_TEXTURE_MIN_FILTER` 和 `GL_TEXTURE_MAG_FILTER`：设置纹理缩小和放大时的过滤方法（如最近邻过滤、线性过滤等）。

6. **解绑纹理**:
   ```cpp
   glBindTexture(GL_TEXTURE_2D, 0);
   ```
   - 通过将纹理 ID 绑定为 0，可以解绑当前绑定的纹理。这是一个好的做法，以避免不小心修改当前纹理。

### 总结

这段代码展示了如何在 OpenGL 中创建一个 2D 纹理并设置其属性。通过生成纹理 ID、提供纹理数据、设置包装和过滤参数，以及最后解绑纹理，为纹理的使用做了准备。这是 OpenGL 纹理处理中的常见流程。

*/