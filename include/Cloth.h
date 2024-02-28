#pragma once

#include <vector>
#include <random>

#include "core.h"
#include "Particle.h"
#include "SpringDamper.h"
#include "Triangle.h"

using namespace std;

class Cloth {
    public:
    vector<Particle*> particles;
    vector<SpringDamper*> springDampers;
    vector<Triangle*> triangles;
    float height;
    float width;
    int num_particles_height = 15;
    int num_particles_width = 15;


    // opengl
    GLuint VAO;
    GLuint VBO_positions, VBO_normals, EBO;

    glm::mat4 model;
    glm::vec3 color;
    glm::vec3 wind;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    Cloth(glm::vec3 wind);
    ~Cloth();
    void applyAllForce(float deltaTime);
    void openGLbind();
    void draw(const glm::mat4& viewProjMtx, GLuint shader);
    void updateNormal();
    void update(float deltaTime);
    void move(glm::vec3 direction);
};