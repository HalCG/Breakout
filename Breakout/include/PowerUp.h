#ifndef POWER_UP_H
#define POWER_UP_H
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "GameObject.h"


// ��ʼ����С
const glm::vec2 POWERUP_SIZE(60.0f, 20.0f);
// ��ʼ����ֱ�����ٶ�
const glm::vec2 VELOCITY(0.0f, 150.0f);

class PowerUp : public GameObject
{
public:
    //Speed, Sticky, Pass-Through, Pad-Size-Increase, Confuse, Chaos.
    std::string Type;       //��Ч��������
    float       Duration;   //�������
    bool        Activated;  //�Ƿ񼤻�

    PowerUp(std::string type, glm::vec3 color, float duration, glm::vec2 position, Texture2D texture)
        : GameObject(position, POWERUP_SIZE, texture, color, VELOCITY), Type(type), Duration(duration), Activated() { }
};

#endif