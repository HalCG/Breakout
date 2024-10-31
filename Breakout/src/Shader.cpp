#include "shader.h"

#include <iostream>

Shader& Shader::Use()
{
    glUseProgram(this->ID);
    return *this;
}

void Shader::Compile(const GLchar* vertexSource, const GLchar* fragmentSource, const GLchar* geometrySource)
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

void Shader::SetFloat(const GLchar* name, GLfloat value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform1f(glGetUniformLocation(this->ID, name), value);
}
void Shader::SetInteger(const GLchar* name, GLint value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform1i(glGetUniformLocation(this->ID, name), value);
}
void Shader::SetVector2f(const GLchar* name, GLfloat x, GLfloat y, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(this->ID, name), x, y);
}
void Shader::SetVector2f(const GLchar* name, const glm::vec2& value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
}
void Shader::SetVector3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
}
void Shader::SetVector3f(const GLchar* name, const glm::vec3& value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
}
void Shader::SetVector4f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
}
void Shader::SetVector4f(const GLchar* name, const glm::vec4& value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
}
void Shader::SetMatrix4(const GLchar* name, const glm::mat4& matrix, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, GL_FALSE, glm::value_ptr(matrix));
}


void Shader::checkCompileErrors(GLuint object, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
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
     void Shader::SetVector4f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader)
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
     void Shader::SetVector4f(const GLchar* name, const glm::vec4& value, GLboolean useShader)
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
     void Shader::SetMatrix4(const GLchar* name, const glm::mat4& matrix, GLboolean useShader)
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