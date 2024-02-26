#include "Cloth.h"

Cloth::Cloth() {
    color = glm::vec3(1.0f, 0.0f, 0.0f);
    model = glm::mat4(1.0f);

    int num_particles_width = 10;
    int num_particles_height = 10;
    float width = 5.0f; // Width of the cloth
    float height = 5.0f; // Height of the cloth

    // Calculate the distance between particles
    float dx = width / (num_particles_width - 1);
    float dy = height / (num_particles_height - 1);

    // Use a single-dimensional vector to store particles
    // particles.resize(num_particles_width * num_particles_height);

    for (int y = 0; y < num_particles_height; ++y) {
        for (int x = 0; x < num_particles_width; ++x) {
            glm::vec3 position = glm::vec3(dx * x - 2.5, dy * y - 1.5, 0);
            bool isFixed = (y == 0); // Only the top row is fixed
            Particle* p = new Particle();
            p->Position = position;
            p->Normal = glm::vec3(0, 0, 0);
            p->Fixed = isFixed;
            p->ind = y * num_particles_height + x; 

            //TODO: tune mass
            p->Mass = 0.1f;


            particles.push_back(p);
            positions.push_back(p->Position);
            normals.push_back(p->Normal);

            // particles[y * num_particles_width + x] = new Particle(position, glm::vec3(0, 0, 0), 1.0f, isFixed);
        }
    }

    // indices
    for (int i = 0; i < num_particles_height - 1; i++) {
        for (int j = 0; j < num_particles_width - 1; j++) {
            int index1 = i * num_particles_width + j;
            int index2 = index1 + 1;
            int index3 = (i + 1) * num_particles_width + j;
            indices.push_back(index1);
            indices.push_back(index2);
            indices.push_back(index3);

            // Assuming you have a structure for Triangle
            Triangle* t = new Triangle();
            t->p0 = particles[index1];
            t->p1 = particles[index2];
            t->p2 = particles[index3];
            triangles.push_back(t);

            int index4 = (i + 1) * num_particles_width + j + 1;
            indices.push_back(index2);
            indices.push_back(index3);
            indices.push_back(index4);

            Triangle* t2 = new Triangle();
            t2->p0 = particles[index2];
            t2->p1 = particles[index4];
            t2->p2 = particles[index3];
            triangles.push_back(t2);
        }
    }

    updateNormal();

    openGLbind();
    // printf("openGLbind!");
}

void Cloth::update(float deltaTime){
    
    // Gravity & Euler -> update
    for(Particle* p : particles){
        // TODO:tune gravity
        glm::vec3 gravity = glm::vec3(0.0f, -9.8f, 0.0f);
        // add gravity
        p->ApplyForce(gravity);
        p->Integrate(deltaTime);
        positions[p->ind] = p->Position;
    }
    updateNormal();
    openGLbind();
    // printf("openGLbind!");
}
Cloth::~Cloth(){
    
}

void Cloth::updateNormal(){
    // reset
    for(int i = 0; i < particles.size(); i++){
        particles[i]->Normal = glm::vec3(0.0f, 0.0f, 0.0f);
        normals[i] = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    // deal with triangles
    for(int i = 0; i < triangles.size(); i++){
        Triangle* tri = triangles[i]; 
        Particle* p0 = tri->p0;
        Particle* p1 = tri->p1;
        Particle* p2 = tri->p2;

        glm::vec3 n0 = p1->Position - p0->Position;
        glm::vec3 n1 = p2->Position - p0->Position;
        glm::vec3 newN = glm::normalize(glm::cross(n0,n1));

        p0->Normal += newN;
        p1->Normal += newN;
        p2->Normal += newN;

        normals[p0->ind] =+ newN;
        normals[p1->ind] =+ newN;
        normals[p2->ind] =+ newN;
    }

    // at the end, normalize all
    for(int i = 0; i < normals.size(); i ++){
        normals[i] = glm::normalize(normals[i]);
    }
}

void Cloth::openGLbind() {
    // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO_positions);
    glGenBuffers(1, &VBO_normals);

    // Bind to the VAO.
    glBindVertexArray(VAO);

    // Bind to the first VBO - We will use it to store the vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Bind to the second VBO - We will use it to store the normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Generate EBO, bind the EBO to the bound VAO and send the data
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // Unbind the VBOs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Cloth::draw(const glm::mat4& viewProjMtx, GLuint shader){
    // actiavte the shader program
    glUseProgram(shader);

    // get the locations and send the uniforms to the shader
    glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
    glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

    // Bind the VAO
    glBindVertexArray(VAO);

    // draw the points using triangles, indexed with the EBO
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // Unbind the VAO and shader program
    glBindVertexArray(0);
    glUseProgram(0);
}