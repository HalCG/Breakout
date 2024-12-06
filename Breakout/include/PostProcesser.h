#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "SpriteRenderer.h"
#include "shader.h"

class PostProcessor
{
public:
    // ���캯������һ����ɫ������Ϳ�߲�������ʼ�� PostProcessor �Ļ���״̬�������ܻ������ڲ�����֡�������FBO�������������Դ��
    PostProcessor(Shader shader, unsigned int width, unsigned int height);
    // ����Ⱦ��Ϸ֮ǰ���ô˺�����Ŀ����׼�����ڴ����֡���������ͨ����󶨱�Ҫ�� FBO��
    void BeginRender();
    // ��Ⱦ��Ϸ����ô˺���������Ⱦ�����ݴ洢����������С���һ�����Ǻ��ڴ���Ч���ܹ���ȷӦ�õĹؼ���
    void EndRender();
    // ��Ⱦ���ڴ���������ı��Σ�����������������Ƶ���Ļ�ϡ��������ʹ�õ�ǰʱ�����������Ӱ����ȾЧ��������ʵ�ֶ�̬��Ч��
    void Render(float time);

    // ���ڼ���Ч����shader
    Shader PostProcessingShader;
    Texture2D Texture;
    unsigned int Width, Height;
    // ��ת�����ҡ�����Ч��
    bool Confuse, Chaos, Shake;

private:
    // render state
    //MSFBO �Ƕ��ز���֡�������Multisampled FBO�������ڿ���ݴ���FBO �ǳ����֡���壬���ڽ����ز�������ɫ���������Ƶ������С�
    unsigned int MSFBO, FBO;
    //�ڴ洢���ز�������ɫ����������Ⱦ�������Renderbuffer Object����
    unsigned int RBO;
    //��������������ڹ�����Ⱦ�ı��Σ���Ļ�ϵĴ��飩������ʾ���ڴ����Ч����
    unsigned int VAO;
    // ����һ��˽�к��������ڳ�ʼ����Ⱦ��ص����ݣ����紴�������� VAO��VBO�����㻺����󣩵ȣ�ʹ�ú��ڴ�������������ȷ��Ⱦ����Ļ��
    void initRenderData();
};

#endif