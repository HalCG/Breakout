#include "ParticleGenerator.h"

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount)
    : shader(shader), texture(texture), amount(amount)
{
    this->init();
}

/// <summary>
/// /更新粒子的位置和生命周期
/// </summary>
/// <param name="dt">自上次更新以来的时间增量</param>
/// <param name="object">生成粒子的物体，一般包含位置和速度</param>
/// <param name="newParticles">新生成的粒子数量</param>
/// <param name="offset">偏移量，用于粒子的初始位置</param>
void ParticleGenerator::Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset)
{
    // 循环添加新粒子，调用 firstUnusedParticle() 获取可用的粒子索引，并通过 respawnParticle() 方法重生粒子
    for (unsigned int i = 0; i < newParticles; ++i)
    {
        int unusedParticle = this->firstUnusedParticle();
        this->respawnParticle(this->particles[unusedParticle], object, offset);
    }
    // 循环更新所有粒子的生命周期和位置。如果粒子仍然存活（Life > 0），则更新其位置和透明度
    for (unsigned int i = 0; i < this->amount; ++i)
    {
        Particle& p = this->particles[i];
        p.Life -= dt;
        if (p.Life > 0.0f)
        {
            p.Position -= p.Velocity * dt;
            p.Color.a -= dt * 2.5f;
        }
    }
}

// 绘制所有活跃粒子
void ParticleGenerator::Draw()
{
    // 使用加法混合来实现发光效果
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    this->shader.Use();
    for (Particle particle : this->particles)
    {
        if (particle.Life > 0.0f)
        {
            this->shader.SetVector2f("offset", particle.Position);
            this->shader.SetVector4f("color", particle.Color);
            this->texture.Bind();
            glBindVertexArray(this->VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    // 重置混合模式为默认状态
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
    // 定义一个四边形的顶点数据，表示粒子的形状
    unsigned int VBO;
    float particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    //生成并设置顶点数组对象（VAO）和顶点缓冲对象（VBO）
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(this->VAO);
    //将顶点数据填入 VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    //启用顶点属性，并指明属性格式
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // 创建指定数量的默认粒子实例，初始化为未使用状态
    for (unsigned int i = 0; i < this->amount; ++i)
        this->particles.push_back(Particle());
}

// 寻找并返回一个未使用的粒子索引
unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle()
{
    // 首先从上次使用的粒子索引开始查找，通常效率较高
    for (unsigned int i = lastUsedParticle; i < this->amount; ++i) {
        if (this->particles[i].Life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }
    // 如果在此范围内找不到未使用的粒子，则进行线性搜索
    for (unsigned int i = 0; i < lastUsedParticle; ++i) {
        if (this->particles[i].Life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }
    // 如果所有粒子都在使用，将返回第一个粒子的索引并将 lastUsedParticle 复位为 0
    lastUsedParticle = 0;
    return 0;
}

//重生粒子并设置其初始状态（颜色及位置有一定随机性）
void ParticleGenerator::respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset)
{
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle.Position = object.Position + random + offset;
    particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.Life = 1.0f;
    particle.Velocity = object.Velocity * 0.1f;
}