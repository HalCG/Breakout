#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "Shader.h"


// 用于存储单个字符（字形）的相关信息
struct Character {
    unsigned int TextureID; // 该字形的纹理 ID，用来在 OpenGL 中绑定对应的纹理
    glm::ivec2   Size;      // 字形的尺寸，使用二维整数向量来表示宽度和高度。
    glm::ivec2   Bearing;   // 字形的偏移，表示从基线到字形的左侧和顶部的距离，用于正确对齐文本。
    unsigned int Advance;   // 水平偏移量，表示绘制下一个字形所需的距离。
};


// 文本渲染器类
class TextRenderer
{
public:
    // 初始化文本渲染器，宽度和高度通常与窗口尺寸相关，用于设置视口等参数。
    TextRenderer(unsigned int width, unsigned int height);
    // 加载指定的字体并创建相应字符的纹理。在这里，通常使用 FreeType 库来处理字体文件。
    void Load(std::string font, unsigned int fontSize);
    // 渲染一段文本，参数包括文本内容、在屏幕上的起始位置、缩放比例和颜色。这个方法会使用预编译的字符数据来绘制实际的文字。
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));

    // 一个字符到其渲染信息 (Character) 的映射表，可以通过字符来快速检索其状态信息。
    std::map<char, Character> Characters;
    // 用于文本渲染的着色器对象，负责处理着色和渲染过程。
    Shader TextShader;

private:
    // 分别是顶点数组对象（Vertex Array Object）和顶点缓冲对象（Vertex Buffer Object），用于管理字符图形数据的存储和渲染。
    unsigned int VAO, VBO;
};

#endif 