#include "Window.h"

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "Model Environment";

// Objects to render
Cube* Window::cube;
// Box* Window::box;
Ground* Window::ground;
Cloth* Window::cloth;
glm::vec3 Window::wind;

// Camera Properties
Camera* Cam;

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;

// Constructors and desctructors
bool Window::initializeProgram() {
    // Create a shader program with a vertex shader and a fragment shader.
    shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

    // Check the shader program.
    if (!shaderProgram) {
        std::cerr << "Failed to initialize shader program" << std::endl;
        return false;
    }

    return true;
}

bool Window::initializeObjects() {
    // Create a cube
    // cube = new Cube();
    // box = new Box();
    ground = new Ground();
    wind = glm::vec3(0.0f, 0.0f, 5.0f);
    cloth = new Cloth(wind);
    // cube = new Cube(glm::vec3(-1, 0, -2), glm::vec3(1, 1, 1));

    return true;
}

void Window::cleanUp() {
    // Deallcoate the objects.
    // delete cube;
    // delete box;
    delete ground;
    delete cloth;

    // Delete the shader program.
    glDeleteProgram(shaderProgram);

    //gui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

// for the Window
GLFWwindow* Window::createWindow(int width, int height) {
    // Initialize GLFW.
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return NULL;
    }

    // 4x antialiasing.
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Apple implements its own version of OpenGL and requires special treatments
    // to make it uses modern OpenGL.
    // Ensure that minimum OpenGL version is 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Enable forward compatibility and allow a modern OpenGL context
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create the GLFW window.
    GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

    // Check if the window could not be created.
    if (!window) {
        std::cerr << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return NULL;
    }

    // Make the context of the window.
    glfwMakeContextCurrent(window);

    // Set swap interval to 1.
    glfwSwapInterval(0);

    // set up the camera
    Cam = new Camera();
    Cam->SetAspect(float(width) / float(height));

    // initialize the interaction variables
    LeftDown = RightDown = false;
    MouseX = MouseY = 0;

    // Call the resize callback to make sure things get drawn immediately.
    Window::resizeCallback(window, width, height);

    return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height) {
#ifdef __APPLE__
    // In case your Mac has a retina display.
    glfwGetFramebufferSize(window, &width, &height);
#endif
    Window::width = width;
    Window::height = height;
    // Set the viewport size.
    glViewport(0, 0, width, height);

    Cam->SetAspect(float(width) / float(height));
}

// update and draw functions
void Window::idleCallback() {
    // Perform any updates as necessary.
    Cam->Update();

    // cube->update();
    // box->update();
    float deltaTime = 1.0f / 1200.0f;
    cloth->update(deltaTime);
    // cloth->update(float(1/60));
    
}

void Window::displayCallback(GLFWwindow* window) {
    // Clear the color and depth buffers.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the object.
    // cube->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
    // box->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
    ground -> draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
    cloth -> draw(Cam->GetViewProjectMtx(), Window::shaderProgram);

    // Gets events, including input such as keyboard and mouse or window resizing.
    render(window);
    glfwPollEvents();
    // Swap buffers.
    glfwSwapBuffers(window);
}

void Window::render(GLFWwindow* window){

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    showWindow(window);

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// add contents
void Window::showWindow(GLFWwindow* window){
    ImGui::Begin("Imgui");
    ImGui::Text("Wind Velocity");  
    ImGui::SliderFloat("Wind x", &cloth->wind.x, -10.0f, 10.0f);
    ImGui::SliderFloat("Wind y", &cloth->wind.y, -10.0f, 10.0f);
    ImGui::SliderFloat("Wind z", &cloth->wind.z, -10.0f, 10.0f);
    // if (ImGui::IsItemActive()) ImGui::CaptureKeyboardFromApp(true);
    ImGui::End();
}

// helper to reset the camera
void Window::resetCamera() {
    Cam->Reset();
    Cam->SetAspect(float(Window::width) / float(Window::height));
}

// callbacks - for Interaction
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    /*
     * TODO: Modify below to add your key callbacks.
     */

    // Check for a key press.

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard) {
        return;
    }


    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                // Close the window. This causes the program to also terminate.
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;

            case GLFW_KEY_R:
                resetCamera();
                break;

            case GLFW_KEY_UP:
                cloth->move(glm::vec3(0.0f, 0.1f , 0.0f));
                break;

            case GLFW_KEY_DOWN:
                cloth->move(glm::vec3(0.0f, -0.1f , 0.0f));
                break;

            case GLFW_KEY_A:
                cloth->move(glm::vec3(-0.1f, 0.0f , 0.0f));
                break;

            case GLFW_KEY_D:
                cloth->move(glm::vec3(0.1f, 0.0f , 0.0f));
                break;

            case GLFW_KEY_W:
                cloth->move(glm::vec3(0.0f, 0.0f , -0.1f));
                break;

            case GLFW_KEY_S:
                cloth->move(glm::vec3(0.0f, 0.0f , 0.1f));
                break;

            case GLFW_KEY_LEFT:{
                // fix a bug by initalize a value for num_particles_width at very beginning
                glm::vec3 o = (cloth->particles[cloth->particles.size()-1]->Position 
                + cloth->particles[cloth->particles.size() - cloth->num_particles_width]->Position) 
                / 2.0f;
                // std::cout << "top right: " << glm::to_string(cloth->particles[cloth->particles.size()-1]->Position) << std::endl;
                // std::cout << "top left: " << glm::to_string(cloth->particles[cloth->particles.size() 
                // - cloth->num_particles_width ]->Position) << std::endl;
                // std::cout << "o: " << glm::to_string(o) << std::endl;
                // // << std::endl;

                for (Particle* p : cloth->particles) {
                    if (p->Fixed) {
                        glm::mat4 model;
                        model = glm::translate(glm::mat4(1.0f), -o);
                        model = glm::rotate(model, glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                        model = glm::translate(model, o);
                        p->Position = model * glm::vec4(p->Position, 1.0);
                    }
                }
                break;
            }

            case GLFW_KEY_RIGHT:{
                // fix a bug by initalize a value for num_particles_width at very beginning
                glm::vec3 o = (cloth->particles[cloth->particles.size()-1]->Position 
                + cloth->particles[cloth->particles.size() - cloth->num_particles_width]->Position) 
                / 2.0f;
                // std::cout << "top right: " << glm::to_string(cloth->particles[cloth->particles.size()-1]->Position) << std::endl;
                // std::cout << "top left: " << glm::to_string(cloth->particles[cloth->particles.size() 
                // - cloth->num_particles_width ]->Position) << std::endl;
                // std::cout << "o: " << glm::to_string(o) << std::endl;
                // // << std::endl;

                for (Particle* p : cloth->particles) {
                    if (p->Fixed) {
                        glm::mat4 model;
                        model = glm::translate(glm::mat4(1.0f), -o);
                        model = glm::rotate(model, glm::radians(-5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                        model = glm::translate(model, o);
                        p->Position = model * glm::vec4(p->Position, 1.0);
                    }
                }
                break;
            }

            default:
                break;
        }
    }
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        LeftDown = (action == GLFW_PRESS);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        RightDown = (action == GLFW_PRESS);
    }
}

void Window::cursor_callback(GLFWwindow* window, double currX, double currY) {
    int maxDelta = 100;
    int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
    int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

    MouseX = (int)currX;
    MouseY = (int)currY;

    // Move camera
    // NOTE: this should really be part of Camera::Update()
    if (LeftDown) {
        const float rate = 1.0f;
        Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
        Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
    }
    if (RightDown) {
        const float rate = 0.005f;
        float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
        Cam->SetDistance(dist);
    }
}