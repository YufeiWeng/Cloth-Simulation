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

    // debug: very big normal
    if(crossSectionalArea > area){
        std::cout<< "v: " << glm::to_string(v) <<std::endl;
        std::cout<< "normalizeV: " << glm::to_string((glm::normalize(v))) <<std::endl;
        std::cout<< "n: " << glm::to_string(normal) <<std::endl;
        std::cout<< "crossSectionalArea: " << crossSectionalArea <<std::endl;
        std::cout<< "AeroF: " <<glm::to_string(AeroF)<<std::endl;
        std::cout<< "---------------------------------------------------"<<std::endl;
    }
    
    p0->ApplyForce(AeroF);
    p1->ApplyForce(AeroF);
    p2->ApplyForce(AeroF);
}