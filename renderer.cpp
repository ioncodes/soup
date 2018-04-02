//
// Created by Luca Marcelli on 02.04.18.
//

#include <GL/gl3w.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <GLFW/glfw3.h>
#include "renderer.h"

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error %d: %s\n", error, description);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    printf("Resizing to %dx%d\n", width, height);
    glViewport(0, 0, width, height);
}

void Renderer::compile_shader(char *file) {
    // Don't leak stuff
    glDeleteShader(m_fragment_shader);
    glDeleteShader(m_vertex_shader);
    glDeleteProgram(m_program);

    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // Fullscreen rectangle
    GLfloat vertices[] = {
            -1.0f,  1.0f, 1.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 1.0f, 1.0f, 1.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create an element array
    GLuint ebo;
    glGenBuffers(1, &ebo);

    GLuint elements[] = {
            0, 1, 2,
            2, 3, 0
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    // Create and compile the vertex shader
    m_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_vertex_shader, 1, &VERTEX, NULL);
    glCompileShader(m_vertex_shader);

    // Create and compile the fragment shader
    m_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    std::ifstream t(file);
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    const char *c_str = str.c_str();
    glShaderSource(m_fragment_shader, 1, &c_str, NULL);
    glCompileShader(m_fragment_shader);

    // Link the vertex and fragment shader into a shader program
    m_program = glCreateProgram();
    glAttachShader(m_program, m_vertex_shader);
    glAttachShader(m_program, m_fragment_shader);
    glBindFragDataLocation(m_program, 0, "outColor");
    glLinkProgram(m_program);
    glDetachShader(m_program, m_fragment_shader);
    glDetachShader(m_program, m_vertex_shader);
    glUseProgram(m_program);

    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(m_program, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
}

bool Renderer::check_shader(char* log) {
    GLint success = 0;
    glGetShaderiv(m_fragment_shader, GL_COMPILE_STATUS, &success);
    GLint max_length = 0;
    glGetShaderiv(m_fragment_shader, GL_INFO_LOG_LENGTH, &max_length);
    if (!success) {
        std::vector<GLchar> errorLog(max_length);
        glGetShaderInfoLog(m_fragment_shader, max_length, &max_length, &errorLog[0]);
        log = &errorLog[0];
        return false;
    }
    return true;
}

bool Renderer::load_renderer(char **log) {
    if (!glfwInit()) {
        log = (char **) "Failed to initialize GLFW";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    m_window = glfwCreateWindow(1280, 720, ".: soup :.", NULL, NULL);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    glfwSetErrorCallback(error_callback);
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    if (gl3wInit()) {
        log = (char **) "Failed to initialize OpenGL";
        return false;
    }
    return true;
}

Renderer::resolution Renderer::get_resolution() {
    int width;
    int height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return resolution {
        .x = (float)width,
        .y = (float)height
    };
}

GLint Renderer::get_uniform_location(const char *name) {
    return glGetUniformLocation(m_program, name);
}

GLFWwindow* Renderer::get_window() {
    return m_window;
}

Renderer::~Renderer() {
    try {
        //glDeleteShader(m_fragment_shader);
        //glDeleteShader(m_vertex_shader);
        //glDeleteProgram(m_program);
    } catch(int e) {}
}
