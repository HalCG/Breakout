#include "shader.h"

#include <glad/glad.h>

#include <iostream>

Shader& Shader::Use()
{
    glUseProgram(this->ID);
    return *this;
}

void Shader::Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
    GLuint sVertex, sFragment, gShader;
    // Vertex Shader
    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertexSource, NULL);
    glCompileShader(sVertex);
    checkCompileErrors(sVertex, "VERTEX");
    // Fragment Shader
    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragmentSource, NULL);
    glCompileShader(sFragment);
    checkCompileErrors(sFragment, "FRAGMENT");
    // If geometry shader source code is given, also compile geometry shader
    if (geometrySource != nullptr)
    {
        gShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gShader, 1, &geometrySource, NULL);
        glCompileShader(gShader);
        checkCompileErrors(gShader, "GEOMETRY");
    }
    // Shader Program
    this->ID = glCreateProgram();
    glAttachShader(this->ID, sVertex);
    glAttachShader(this->ID, sFragment);
    if (geometrySource != nullptr)
        glAttachShader(this->ID, gShader);
    glLinkProgram(this->ID);
    checkCompileErrors(this->ID, "PROGRAM");
    // Delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
    if (geometrySource != nullptr)
        glDeleteShader(gShader);
}

void Shader::SetFloat(const char* name, float value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform1f(glGetUniformLocation(this->ID, name), value);
}
void Shader::SetInteger(const char* name, GLint value, bool useShader)
{
    if (useShader)
        this->Use();

    std::cout << glGetUniformLocation(this->ID, name) << std::endl;
    glUniform1i(glGetUniformLocation(this->ID, name), value);
}
void Shader::SetVector2f(const char* name, float x, float y, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(this->ID, name), x, y);
}
void Shader::SetVector2f(const char* name, const glm::vec2& value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
}
void Shader::SetVector3f(const char* name, float x, float y, float z, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
}
void Shader::SetVector3f(const char* name, const glm::vec3& value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
}
void Shader::SetVector4f(const char* name, float x, float y, float z, float w, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
}
void Shader::SetVector4f(const char* name, const glm::vec4& value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
}
void Shader::SetMatrix4(const char* name, const glm::mat4& matrix, bool useShader)
{
    if (useShader)
        this->Use();
    glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, GL_FALSE, glm::value_ptr(matrix));
}


void Shader::checkCompileErrors(GLuint object, std::string type)
{
    GLint success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
}


/*
用于管理OpenGL着色器的类的一部分。它包括设置着色器程序中统一变量的方法。下面是每个方法的详细解释，包括参数和功能：

1. **SetVector4f（重载方法）**：
   - **签名**：
     ```cpp
     void Shader::SetVector4f(const char* name, float x, float y, float z, float w, bool useShader)
     ```
   - **参数**：
     - `name`：着色器中统一变量的名称。
     - `x`、`y`、`z`、`w`：四个浮点数分量。
     - `useShader`：一个布尔值，指示在设置统一变量之前是否激活着色器。
   - **功能**：
     - 如果`useShader`为真，则使用`this->Use()`激活着色器程序。
     - 调用`glUniform4f`函数设置着色器中指定名称的四分量向量统一变量。

2. **SetVector4f（使用glm::vec4的重载方法）**：
   - **签名**：
     ```cpp
     void Shader::SetVector4f(const char* name, const glm::vec4& value, bool useShader)
     ```
   - **参数**：
     - `name`：着色器中统一变量的名称。
     - `value`：一个对`glm::vec4`对象的引用，包含四个浮点数分量。
     - `useShader`：一个布尔值，指示是否激活着色器。
   - **功能**：
     - 与前一个方法类似，若`useShader`为真，则激活着色器。
     - 使用`glUniform4f`设置统一变量，使用`glm::vec4`对象的各个分量。

3. **SetMatrix4**：
   - **签名**：
     ```cpp
     void Shader::SetMatrix4(const char* name, const glm::mat4& matrix, bool useShader)
     ```
   - **参数**：
     - `name`：着色器中用于矩阵的统一变量的名称。
     - `matrix`：对`glm::mat4`对象的引用，表示一个4x4矩阵。
     - `useShader`：一个布尔值，指示是否激活着色器。
   - **功能**：
     - 检查`useShader`，如有必要则激活着色器。
     - 调用`glUniformMatrix4fv`设置矩阵统一变量，使用`glm::value_ptr(matrix)`获取矩阵数据的指针，以供OpenGL使用。

总体而言，这个类是一个有效的OpenGL着色器统一变量的管理方式，使您能够轻松地在着色器程序中设置向量和矩阵值，并提供在需要时自动激活着色器的选项。以下是对该实现的一些其他考虑：

- **错误检查**：考虑在`glGetUniformLocation`之后添加错误处理，以检查统一变量的位置是否有效（即是否返回-1）。
- **性能优化**：如果您经常在着色器之间切换，可以缓存`glGetUniformLocation`的结果，这样可以通过减少对该函数的调用次数来提高性能。
- **上下文管理**：确保在调用这些函数时OpenGL上下文是当前的，尤其是在使用多个线程的情况下。

*/


/*
`glUniform1i` 是 OpenGL 中用于设置着色器中 uniform 变量的函数。具体来说，它用于将一个整型值传递给一个 uniform 变量，这个变量通常是在着色器程序中定义的。以下是该函数的具体作用分析：

### 函数参数解析

1. **glGetUniformLocation(this->ID, name)**:
   - 这个函数用于获取指定着色器程序中 uniform 变量的地址（位置）。
   - `this->ID` 是着色器程序的 ID，通常在你编译和链接该着色器后得到。
   - `name` 是 uniform 变量的名称，字符串类型，必须与着色器中定义的名称完全相同。

2. **value**:
   - 这是要传递给 uniform 变量的整型值。`glUniform1i` 函数会将这个值发送到 GPU，以便在绘制时使用。

### 整个调用过程

- 首先，`glGetUniformLocation` 获取 uniform 变量的位置（即其在着色器中的位置）。
- 然后，`glUniform1i` 根据这个位置将 `value` 的值传递给相应的 uniform 变量。

### 具体作用

在实际应用中，uniform 变量通常用于动态控制着色器的行为，比如：

- **传递纹理单元**：在使用纹理的着色器中，你可能会用到这个来指定哪个纹理单元（如 `sampler2D` 的 uniform）应该绑定到 GPU 的纹理单元上。比如，如果你有一个纹理在 `GL_TEXTURE` 位置，你可以设置 `uniform` 为 。

- **控制参数**：其他情况下，可以使用 uniform 变量来传递一些常量参数，如颜色、光源的位置等。

### 示例

假设在你的着色器中有一个定义如下的 uniform 变量：

```glsl
uniform sampler2D myTexture;
```

在主程序中，你可能会这样设置这个 uniform：

```cpp
// 绑定纹理到 GL_TEXTURE
glActiveTexture(GL_TEXTURE);
glBindTexture(GL_TEXTURE_2D, textureID);

// 告诉着色器使用哪个纹理单元
glUniform1i(glGetUniformLocation(this->ID, "myTexture"), );
```

在这个例子中，你将纹理单元  的值传递给名为 `myTexture` 的 uniform 变量，从而在着色器中可以正确访问绑定到该纹理单元的纹理。

### 总结

`glUniform1i` 的作用是将整型值设置到着色器的 uniform 变量中，允许你在渲染过程中动态地控制着色器的行为。
*/