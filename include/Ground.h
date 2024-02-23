#pragma once

#include <vector>

#include "core.h"

class Ground {
public:
    GLuint VAO;
    GLuint VBO_positions, VBO_normals, EBO;

    glm::mat4 model;
    glm::vec3 color;

    // Cube Information
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    Ground();
    ~Ground();

    void draw(const glm::mat4& viewProjMtx, GLuint shader);
    void update();
};

// [main] Building folder: cse169_starter_mac_project4 
// [build] Starting build
// [proc] Executing command: /opt/homebrew/bin/cmake --build /Users/yufeiweng/Downloads/cse169_starter_mac_project4/build --config Debug --target all -j 10 --
// [build] [  0%] Built target CopyShaders
// [build] [ 14%] Building CXX object CMakeFiles/menv.dir/src/Window.cpp.o
// [build] [ 28%] Linking CXX executable menv
// [build] Undefined symbols for architecture arm64:
// [build]   "Ground::Ground()", referenced from:
// [build]       Window::initializeObjects() in Window.cpp.o
// [build] ld: symbol(s) not found for architecture arm64
// [build] clang: error: linker command failed with exit code 1 (use -v to see invocation)
// [build] make[2]: *** [menv] Error 1
// [build] make[1]: *** [CMakeFiles/menv.dir/all] Error 2
// [build] make: *** [all] Error 2
// [proc] The command: /opt/homebrew/bin/cmake --build /Users/yufeiweng/Downloads/cse169_starter_mac_project4/build --config Debug --target all -j 10 -- exited with code: 2
// [driver] Build completed: 00:00:00.868
// [build] Build finished with exit code 2

