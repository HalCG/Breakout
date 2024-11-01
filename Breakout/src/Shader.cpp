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
���ڹ���OpenGL��ɫ�������һ���֡�������������ɫ��������ͳһ�����ķ�����������ÿ����������ϸ���ͣ����������͹��ܣ�

1. **SetVector4f�����ط�����**��
   - **ǩ��**��
     ```cpp
     void Shader::SetVector4f(const char* name, float x, float y, float z, float w, bool useShader)
     ```
   - **����**��
     - `name`����ɫ����ͳһ���������ơ�
     - `x`��`y`��`z`��`w`���ĸ�������������
     - `useShader`��һ������ֵ��ָʾ������ͳһ����֮ǰ�Ƿ񼤻���ɫ����
   - **����**��
     - ���`useShader`Ϊ�棬��ʹ��`this->Use()`������ɫ������
     - ����`glUniform4f`����������ɫ����ָ�����Ƶ��ķ�������ͳһ������

2. **SetVector4f��ʹ��glm::vec4�����ط�����**��
   - **ǩ��**��
     ```cpp
     void Shader::SetVector4f(const char* name, const glm::vec4& value, bool useShader)
     ```
   - **����**��
     - `name`����ɫ����ͳһ���������ơ�
     - `value`��һ����`glm::vec4`��������ã������ĸ�������������
     - `useShader`��һ������ֵ��ָʾ�Ƿ񼤻���ɫ����
   - **����**��
     - ��ǰһ���������ƣ���`useShader`Ϊ�棬�򼤻���ɫ����
     - ʹ��`glUniform4f`����ͳһ������ʹ��`glm::vec4`����ĸ���������

3. **SetMatrix4**��
   - **ǩ��**��
     ```cpp
     void Shader::SetMatrix4(const char* name, const glm::mat4& matrix, bool useShader)
     ```
   - **����**��
     - `name`����ɫ�������ھ����ͳһ���������ơ�
     - `matrix`����`glm::mat4`��������ã���ʾһ��4x4����
     - `useShader`��һ������ֵ��ָʾ�Ƿ񼤻���ɫ����
   - **����**��
     - ���`useShader`�����б�Ҫ�򼤻���ɫ����
     - ����`glUniformMatrix4fv`���þ���ͳһ������ʹ��`glm::value_ptr(matrix)`��ȡ�������ݵ�ָ�룬�Թ�OpenGLʹ�á�

������ԣ��������һ����Ч��OpenGL��ɫ��ͳһ�����Ĺ���ʽ��ʹ���ܹ����ɵ�����ɫ�����������������;���ֵ�����ṩ����Ҫʱ�Զ�������ɫ����ѡ������ǶԸ�ʵ�ֵ�һЩ�������ǣ�

- **������**��������`glGetUniformLocation`֮����Ӵ������Լ��ͳһ������λ���Ƿ���Ч�����Ƿ񷵻�-1����
- **�����Ż�**���������������ɫ��֮���л������Ի���`glGetUniformLocation`�Ľ������������ͨ�����ٶԸú����ĵ��ô�����������ܡ�
- **�����Ĺ���**��ȷ���ڵ�����Щ����ʱOpenGL�������ǵ�ǰ�ģ���������ʹ�ö���̵߳�����¡�

*/


/*
`glUniform1i` �� OpenGL ������������ɫ���� uniform �����ĺ�����������˵�������ڽ�һ������ֵ���ݸ�һ�� uniform �������������ͨ��������ɫ�������ж���ġ������Ǹú����ľ������÷�����

### ������������

1. **glGetUniformLocation(this->ID, name)**:
   - ����������ڻ�ȡָ����ɫ�������� uniform �����ĵ�ַ��λ�ã���
   - `this->ID` ����ɫ������� ID��ͨ�������������Ӹ���ɫ����õ���
   - `name` �� uniform ���������ƣ��ַ������ͣ���������ɫ���ж����������ȫ��ͬ��

2. **value**:
   - ����Ҫ���ݸ� uniform ����������ֵ��`glUniform1i` �����Ὣ���ֵ���͵� GPU���Ա��ڻ���ʱʹ�á�

### �������ù���

- ���ȣ�`glGetUniformLocation` ��ȡ uniform ������λ�ã���������ɫ���е�λ�ã���
- Ȼ��`glUniform1i` �������λ�ý� `value` ��ֵ���ݸ���Ӧ�� uniform ������

### ��������

��ʵ��Ӧ���У�uniform ����ͨ�����ڶ�̬������ɫ������Ϊ�����磺

- **��������Ԫ**����ʹ���������ɫ���У�����ܻ��õ������ָ���ĸ�����Ԫ���� `sampler2D` �� uniform��Ӧ�ð󶨵� GPU ������Ԫ�ϡ����磬�������һ�������� `GL_TEXTURE` λ�ã���������� `uniform` Ϊ ��

- **���Ʋ���**����������£�����ʹ�� uniform ����������һЩ��������������ɫ����Դ��λ�õȡ�

### ʾ��

�����������ɫ������һ���������µ� uniform ������

```glsl
uniform sampler2D myTexture;
```

���������У�����ܻ������������ uniform��

```cpp
// ������ GL_TEXTURE
glActiveTexture(GL_TEXTURE);
glBindTexture(GL_TEXTURE_2D, textureID);

// ������ɫ��ʹ���ĸ�����Ԫ
glUniform1i(glGetUniformLocation(this->ID, "myTexture"), );
```

����������У��㽫����Ԫ  ��ֵ���ݸ���Ϊ `myTexture` �� uniform �������Ӷ�����ɫ���п�����ȷ���ʰ󶨵�������Ԫ������

### �ܽ�

`glUniform1i` �������ǽ�����ֵ���õ���ɫ���� uniform �����У�����������Ⱦ�����ж�̬�ؿ�����ɫ������Ϊ��
*/