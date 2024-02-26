#pragma once

#include "core.h"

class Particle{
    public:
    glm::vec3 NetF;
    glm::vec3 Position, Normal;
    glm::vec3 Velocity;
    float Mass;
    bool Fixed;
    int ind;

    Particle();
    ~Particle();

    void ApplyForce(glm::vec3 &f);
    void Integrate(float deltaTime);
};