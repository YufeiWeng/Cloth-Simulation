#include "Cloth.h"

Cloth::Cloth(glm::vec3 wind) {
    color = glm::vec3(1.0f, 0.0f, 0.0f);
    model = glm::mat4(1.0f);
    this->wind = wind;

    int num_particles_width = 11;
    int num_particles_height = 11;
    float width = 5.0f; // Width of the cloth
    float height = 5.0f; // Height of the cloth

    // Calculate the distance between particles
    float dx = width / (num_particles_width - 1);
    float dy = height / (num_particles_height - 1);

    // Use a single-dimensional vector to store particles
    // particles.resize(num_particles_width * num_particles_height);

    for (int y = 0; y < num_particles_height; ++y) {
        for (int x = 0; x < num_particles_width; ++x) {

            // generate z in range [-0.0001, 0.0001]
            static std::mt19937 gen(std::random_device{}());
            static std::uniform_real_distribution<float> distr(-0.0001f, 0.0001f);
            float z = distr(gen);

            glm::vec3 position = glm::vec3(dx * x - 2.5, dy * y - 1.5, z);
            bool isFixed = (y == num_particles_height-1); // Only the top row is fixed
            Particle* p = new Particle();
            p->Position = position;
            p->Normal = glm::vec3(0, 0, 0);
            p->Fixed = isFixed;
            p->ind = y * num_particles_height + x; 

            //TODO: tune mass
            p->Mass = 0.75f;


            particles.push_back(p);
            positions.push_back(p->Position);
            normals.push_back(p->Normal);

            // particles[y * num_particles_width + x] = new Particle(position, glm::vec3(0, 0, 0), 1.0f, isFixed);
        }
    }

    // Iterate through each row and column to setup spring dampers
    //TODO: Do we need bending forces(i.e. add more SpringDampers....)
    for (int row = 0; row < num_particles_height; row++) {
        for (int col = 0; col < num_particles_width; col++) {
            // Vertical spring - connect current particle with the one directly below, if not on the bottom row
            if (row < num_particles_height - 1) {
                auto* verticalSpring = new SpringDamper();
                verticalSpring->P1 = particles[row * num_particles_width + col];
                verticalSpring->P2 = particles[(row + 1) * num_particles_width + col];
                verticalSpring->RestLength = glm::distance(verticalSpring->P1->Position, verticalSpring->P2->Position);
                springDampers.push_back(verticalSpring);
            }
            
            // Horizontal spring - connect current particle with the one to the right, if not on the rightmost column
            if (col < num_particles_width - 1) {
                auto* horizontalSpring = new SpringDamper();
                horizontalSpring->P1 = particles[row * num_particles_width + col];
                horizontalSpring->P2 = particles[row * num_particles_width + (col + 1)];
                horizontalSpring->RestLength = glm::distance(horizontalSpring->P1->Position, horizontalSpring->P2->Position);
                springDampers.push_back(horizontalSpring);
            }
            
            // Diagonal spring - for shear resistance, connect diagonally to the bottom-right, if not on bottom row or rightmost column
            if (row < num_particles_height - 1 && col < num_particles_width - 1) {
                auto* diagonalSpring = new SpringDamper();
                diagonalSpring->P1 = particles[row * num_particles_width + col];
                diagonalSpring->P2 = particles[(row + 1) * num_particles_width + (col + 1)];
                diagonalSpring->RestLength = glm::distance(diagonalSpring->P1->Position, diagonalSpring->P2->Position);
                springDampers.push_back(diagonalSpring);
            }
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
            glm::vec3 n0 = t->p1->Position - t->p0->Position;
            glm::vec3 n1 = t->p2->Position - t->p0->Position;
            t->area = 0.5f * glm::length(glm::cross(n0,n1));
            // std::cout<< "Area1: " <<glm::to_string(t->area)<<std::endl;
            triangles.push_back(t);

            int index4 = (i + 1) * num_particles_width + j + 1;
            indices.push_back(index2);
            indices.push_back(index3);
            indices.push_back(index4);

            Triangle* t2 = new Triangle();
            t2->p0 = particles[index2];
            t2->p1 = particles[index4];
            t2->p2 = particles[index3];
            glm::vec3 n2 = t2->p1->Position - t2->p0->Position;
            glm::vec3 n3 = t2->p2->Position - t2->p0->Position;
            t2->area = 0.5f * glm::length(glm::cross(n2,n3));
            triangles.push_back(t2);
        }
    }

    updateNormal();

    openGLbind();
    // printf("openGLbind!");
}


void Cloth::update(float deltaTime){
    
    // applyGravity;
    for (Particle* p : particles){
        glm::vec3 gravity = glm::vec3(0.0f, -9.8f, 0.0f);
        // add gravity
        p->ApplyForce(gravity);
    }

    // SpringDamper
    for (SpringDamper* sd : springDampers){
        // TODO: tune constants
        sd -> ComputeForce();
    }

    // Aerodynamic Force
    for (Triangle* tri : triangles){
        //The fluid density 𝜌 of air at 15o C and a pressure of 101.325 kPa
        //(14.696 psi) is 1.225 kg/m3 and is used as a common default value
        tri -> computeAeroForce(wind, 1.225f);
    }

    applyAllForce(deltaTime);

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
        tri -> normal = newN;

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
    for(int i = 0; i < particles.size(); i++){
        particles[i]->Normal = glm::normalize(particles[i]->Normal);
    }
}

void Cloth::applyAllForce(float deltaTime){
    for (Particle* p : particles){
        p->Integrate(deltaTime);
        positions[p->ind] = p->Position;
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
    // std::cout << "draw" << std::endl;
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