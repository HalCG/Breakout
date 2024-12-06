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
    // 构造函数接受一个着色器对象和宽高参数，初始化 PostProcessor 的基本状态。它可能还会在内部创建帧缓冲对象（FBO）和其他相关资源。
    PostProcessor(Shader shader, unsigned int width, unsigned int height);
    // 在渲染游戏之前调用此函数，目的是准备后期处理的帧缓冲操作，通常会绑定必要的 FBO。
    void BeginRender();
    // 渲染游戏后调用此函数，将渲染的数据存储到纹理对象中。这一步骤是后期处理效果能够正确应用的关键。
    void EndRender();
    // 渲染后期处理的纹理四边形，即将处理后的纹理绘制到屏幕上。这个函数使用当前时间参数来可能影响渲染效果，例如实现动态特效。
    void Render(float time);

    // 用于计算效果的shader
    Shader PostProcessingShader;
    Texture2D Texture;
    unsigned int Width, Height;
    // 翻转、混乱、抖动效果
    bool Confuse, Chaos, Shake;

private:
    // render state
    //MSFBO 是多重采样帧缓冲对象（Multisampled FBO），用于抗锯齿处理；FBO 是常规的帧缓冲，用于将多重采样的颜色缓冲区复制到纹理中。
    unsigned int MSFBO, FBO;
    //于存储多重采样的颜色缓冲区的渲染缓冲对象（Renderbuffer Object）。
    unsigned int RBO;
    //顶点数组对象，用于管理渲染四边形（屏幕上的大精灵），以显示后期处理的效果。
    unsigned int VAO;
    // 这是一个私有函数，用于初始化渲染相关的数据，比如创建和配置 VAO、VBO（顶点缓冲对象）等，使得后期处理的纹理可以正确渲染到屏幕上
    void initRenderData();
};

#endif