#pragma once

#include <vector>

#include "core.h"
#include "Particle.h"

class SpringDamper{
    public:
    float SpringConstant;
    float DampingConstant;
    float RestLength;
    Particle *P1, *P2;
    

    void ComputeForce();
    SpringDamper();
    ~SpringDamper();
};