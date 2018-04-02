#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include "renderer.h"

Renderer renderer;

int main(int argc, char** argv) {
    renderer = Renderer();
    char* log;
    if(!renderer.load_renderer(&log)) {
        std::cout << &log << std::endl;
        return -1;
    }

    const GLubyte* render = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    std::string log_render = std::string("Renderer: ").append((char*)render);
    std::string log_version = std::string("OpenGL version supported: ").append((char*)version);
    std::cout << log_render << std::endl;
    std::cout << log_version << std::endl;

    char* file = argv[1];
    renderer.compile_shader(file);

    GLint resolution_loc = renderer.get_uniform_location("resolution");
    GLint time_loc = renderer.get_uniform_location("time");
    float time = 0.00;

    auto resolution = renderer.get_resolution();

    auto window = renderer.get_window();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glUniform2f(resolution_loc, resolution.x, resolution.y);
        glUniform1f(time_loc, time);

        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);

        time += 0.01;
    }

    glfwTerminate();
}