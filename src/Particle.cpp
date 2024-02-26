#include "Particle.h"

Particle::Particle(){
    NetF = glm::vec3(0.0f, 0.0f, 0.0f);
    Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
}

Particle::~Particle(){

}

void Particle::ApplyForce(glm::vec3 &f){
    NetF += f;
}


void Particle::Integrate(float deltaTime) {
    if (!Fixed) {
        glm::vec3 accel = (1/Mass) * NetF; // Apply Newton’s Second Law (f=ma)
        Velocity += accel * deltaTime; // Forward Euler integration to get new Velocity
        Position  += Velocity * deltaTime; // Backward Euler integration to get new Position 

        // check ground
        // TODO: friction?
        if(Position.y < -2){
            Position.y = -2 + 0.01;
            glm::vec3 impulse = deltaTime * NetF;
            ApplyImpulse(impulse);
        }
            
        NetF = glm::vec3(0.0f, 0.0f, 0.0f); // Zero force out so next frame will start fresh
    }
}


void Particle::ApplyImpulse(const glm::vec3& imp) {
    Velocity+=imp/Mass;
}