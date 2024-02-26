#include "Triangle.h"

Triangle::Triangle(){
    dragCoeff = 1.28f;
}

Triangle::~Triangle(){
    
}

void Triangle::computeAeroForce(glm::vec3 fluidVelocity, float fluidDensity){
    glm::vec3 vSurface = (p0->Velocity + p1->Velocity + p2->Velocity) / 3.0f;
    glm::vec3 v = vSurface - fluidVelocity;
    float crossSectionalArea = area * glm::dot((glm::normalize(v)), normal);
    
    glm::vec3 AeroF = (-0.5f * fluidDensity * glm::length(v) * glm::length(v) 
        * dragCoeff * crossSectionalArea * normal) / 3.0f;
    
    p0->ApplyForce(AeroF);
    p1->ApplyForce(AeroF);
    p2->ApplyForce(AeroF);
}