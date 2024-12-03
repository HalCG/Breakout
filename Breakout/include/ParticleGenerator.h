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


// �����ҪĿ���ǹ�������ϵͳ�����ܹ����ɡ����º���Ⱦ������ӣ���������Ҫ��̬Ч���ĳ�����������������������Ӿ�Ч����
class ParticleGenerator
{
public:
    ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
    // ��������ϵͳ��״̬��������������Ӻ͸����������ӵ��������ں�λ�á�
    void Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    void Draw();
private:
    // state
    std::vector<Particle> particles;
    unsigned int amount;

    // ��Ⱦ״̬
    Shader shader;
    Texture2D texture;
    unsigned int VAO;

    // ��ʼ����������������Ļ������Ͷ������ԡ�ͨ���ڹ��캯���е��ã������� OpenGL ��״̬����Դ��
    void init();
    // ���ص�ǰδʹ�����ӵ�������δʹ�õ�����ͨ�������������ѽ��������ӣ�Life <= 0.0f�������û��δʹ�õ����ӣ��򷵻� 0��
    unsigned int firstUnusedParticle();
    // ����һ�����ӣ����������ʼ״̬��λ�á���ɫ���������ڵȣ���
    void respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

#endif