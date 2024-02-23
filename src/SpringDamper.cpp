#include "SpringDamper.h"

SpringDamper::SpringDamper(){

}

SpringDamper::~SpringDamper(){

}

void SpringDamper::ComputeForce(){
    glm::vec3 eStar = P2->Position - P1->Position;
    float l = glm::length(eStar);
    glm::vec3 e = glm::normalize(eStar);

    float VClose = glm::dot((P1->Velocity - P2->Velocity), e);
    float f = - SpringConstant * (RestLength - l) - DampingConstant * VClose;
    glm::vec3 f1 = f * e;
    glm::vec3 f2 = -f1;

    P1->NetF += f1;
    P2->NetF += f2;
}
