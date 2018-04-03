#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include "renderer.h"

Renderer renderer;

int main(int argc, char** argv) {
    renderer = Renderer();
    std::string log = renderer.load_renderer();
    if(log.length() > 0) {
        std::cerr << log << std::endl;
    }

    renderer.print_renderer();

    char* file = argv[1];
    renderer.compile_shader(file);

    std::string shader_log = renderer.check_shader();
    if(shader_log.length() > 0) {
        std::cerr << shader_log << std::endl;
    }

    GLint resolution_loc = renderer.get_uniform_location("resolution");
    GLint time_loc = renderer.get_uniform_location("time");
    float time = 0.00;

    auto resolution = renderer.get_resolution();

    auto window = renderer.get_window();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glUniform2f(resolution_loc, resolution.x, resolution.y);
        glUniform1f(time_loc, time);

        renderer.draw_buffer();

        time += 0.01;
    }

    glfwTerminate();
}