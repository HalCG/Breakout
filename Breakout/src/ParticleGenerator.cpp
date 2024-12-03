#include "ParticleGenerator.h"

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount)
    : shader(shader), texture(texture), amount(amount)
{
    this->init();
}

/// <summary>
/// /�������ӵ�λ�ú���������
/// </summary>
/// <param name="dt">���ϴθ���������ʱ������</param>
/// <param name="object">�������ӵ����壬һ�����λ�ú��ٶ�</param>
/// <param name="newParticles">�����ɵ���������</param>
/// <param name="offset">ƫ�������������ӵĳ�ʼλ��</param>
void ParticleGenerator::Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset)
{
    // ѭ����������ӣ����� firstUnusedParticle() ��ȡ���õ�������������ͨ�� respawnParticle() ������������
    for (unsigned int i = 0; i < newParticles; ++i)
    {
        int unusedParticle = this->firstUnusedParticle();
        this->respawnParticle(this->particles[unusedParticle], object, offset);
    }
    // ѭ�������������ӵ��������ں�λ�á����������Ȼ��Life > 0�����������λ�ú�͸����
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

// �������л�Ծ����
void ParticleGenerator::Draw()
{
    // ʹ�üӷ������ʵ�ַ���Ч��
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
    // ���û��ģʽΪĬ��״̬
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
    // ����һ���ı��εĶ������ݣ���ʾ���ӵ���״
    unsigned int VBO;
    float particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    //���ɲ����ö����������VAO���Ͷ��㻺�����VBO��
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(this->VAO);
    //�������������� VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    //���ö������ԣ���ָ�����Ը�ʽ
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // ����ָ��������Ĭ������ʵ������ʼ��Ϊδʹ��״̬
    for (unsigned int i = 0; i < this->amount; ++i)
        this->particles.push_back(Particle());
}

// Ѱ�Ҳ�����һ��δʹ�õ���������
unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle()
{
    // ���ȴ��ϴ�ʹ�õ�����������ʼ���ң�ͨ��Ч�ʽϸ�
    for (unsigned int i = lastUsedParticle; i < this->amount; ++i) {
        if (this->particles[i].Life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }
    // ����ڴ˷�Χ���Ҳ���δʹ�õ����ӣ��������������
    for (unsigned int i = 0; i < lastUsedParticle; ++i) {
        if (this->particles[i].Life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }
    // ����������Ӷ���ʹ�ã������ص�һ�����ӵ��������� lastUsedParticle ��λΪ 0
    lastUsedParticle = 0;
    return 0;
}

//�������Ӳ��������ʼ״̬����ɫ��λ����һ������ԣ�
void ParticleGenerator::respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset)
{
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle.Position = object.Position + random + offset;
    particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.Life = 1.0f;
    particle.Velocity = object.Velocity * 0.1f;
}