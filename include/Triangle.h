#pragma once

#include <vector>

#include "core.h"
#include "Particle.h"

using namespace std;

class Triangle{
    public:
    Particle *p0, *p1, *p2;
    glm::vec3 normal;
    float area;
    float dragCoeff;

    Triangle();
    ~Triangle();
    void computeAeroForce(glm::vec3 fluidVelocity, float fluidDensity);
};

