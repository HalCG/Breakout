#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"
#include "GameObject.h"

struct Particle {
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    float     Life;

    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};


// 类的主要目的是管理粒子系统。它能够生成、更新和渲染多个粒子，适用于需要动态效果的场景，例如烟雾、火焰或其他视觉效果。
class ParticleGenerator
{
public:
    ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
    // 更新粒子系统的状态，包括添加新粒子和更新现有粒子的生命周期和位置。
    void Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    void Draw();
private:
    // state
    std::vector<Particle> particles;
    unsigned int amount;

    // 渲染状态
    Shader shader;
    Texture2D texture;
    unsigned int VAO;

    // 初始化粒子生成器所需的缓冲区和顶点属性。通常在构造函数中调用，以设置 OpenGL 的状态和资源。
    void init();
    // 返回当前未使用粒子的索引。未使用的粒子通常是生命周期已结束的粒子（Life <= 0.0f）。如果没有未使用的粒子，则返回 0。
    unsigned int firstUnusedParticle();
    // 重生一个粒子，并设置其初始状态（位置、颜色、生命周期等）。
    void respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

#endif