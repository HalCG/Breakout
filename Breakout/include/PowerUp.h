#ifndef POWER_UP_H
#define POWER_UP_H
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "GameObject.h"


// 初始化大小
const glm::vec2 POWERUP_SIZE(60.0f, 20.0f);
// 初始化垂直方向速度
const glm::vec2 VELOCITY(0.0f, 150.0f);

class PowerUp : public GameObject
{
public:
    //Speed, Sticky, Pass-Through, Pad-Size-Increase, Confuse, Chaos.
    std::string Type;       //特效或功能类型
    float       Duration;   //存活周期
    bool        Activated;  //是否激活

    PowerUp(std::string type, glm::vec3 color, float duration, glm::vec2 position, Texture2D texture)
        : GameObject(position, POWERUP_SIZE, texture, color, VELOCITY), Type(type), Duration(duration), Activated() { }
};

#endif