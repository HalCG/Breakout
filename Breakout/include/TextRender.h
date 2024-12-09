#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "Shader.h"


// ���ڴ洢�����ַ������Σ��������Ϣ
struct Character {
    unsigned int TextureID; // �����ε����� ID�������� OpenGL �а󶨶�Ӧ������
    glm::ivec2   Size;      // ���εĳߴ磬ʹ�ö�ά������������ʾ��Ⱥ͸߶ȡ�
    glm::ivec2   Bearing;   // ���ε�ƫ�ƣ���ʾ�ӻ��ߵ����ε����Ͷ����ľ��룬������ȷ�����ı���
    unsigned int Advance;   // ˮƽƫ��������ʾ������һ����������ľ��롣
};


// �ı���Ⱦ����
class TextRenderer
{
public:
    // ��ʼ���ı���Ⱦ������Ⱥ͸߶�ͨ���봰�ڳߴ���أ����������ӿڵȲ�����
    TextRenderer(unsigned int width, unsigned int height);
    // ����ָ�������岢������Ӧ�ַ������������ͨ��ʹ�� FreeType �������������ļ���
    void Load(std::string font, unsigned int fontSize);
    // ��Ⱦһ���ı������������ı����ݡ�����Ļ�ϵ���ʼλ�á����ű�������ɫ�����������ʹ��Ԥ������ַ�����������ʵ�ʵ����֡�
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));

    // һ���ַ�������Ⱦ��Ϣ (Character) ��ӳ�������ͨ���ַ������ټ�����״̬��Ϣ��
    std::map<char, Character> Characters;
    // �����ı���Ⱦ����ɫ�����󣬸�������ɫ����Ⱦ���̡�
    Shader TextShader;

private:
    // �ֱ��Ƕ����������Vertex Array Object���Ͷ��㻺�����Vertex Buffer Object�������ڹ����ַ�ͼ�����ݵĴ洢����Ⱦ��
    unsigned int VAO, VBO;
};

#endif 