#pragma once

#include <vector>

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
    int num_particles_height;
    int num_particles_width;


    // opengl
    GLuint VAO;
    GLuint VBO_positions, VBO_normals, EBO;

    glm::mat4 model;
    glm::vec3 color;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    Cloth();
    ~Cloth();
    void render();
    void draw(const glm::mat4& viewProjMtx, GLuint shader);
    void updateNormal();
    void update(float deltaTime);
};