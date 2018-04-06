//
// Created by Luca Marcelli on 02.04.18.
//

#include <GL/gl3w.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <GLFW/glfw3.h>
#include <cmath>
#include "renderer.h"

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error %d: %s\n", error, description);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    printf("Resizing to %dx%d\n", width, height);
    glViewport(0, 0, width, height);
    // glUniform2f(resolution_loc, resolution.x, resolution.y);
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
    ifstream t(file);
    string str((istreambuf_iterator<char>(t)),
                    istreambuf_iterator<char>());
    const char *c_str = str.c_str();
    glShaderSource(m_fragment_shader, 1, &c_str, NULL);
    glCompileShader(m_fragment_shader);

    // Link the vertex and fragment shader into a shader program
    m_program = glCreateProgram();
    glAttachShader(m_program, m_vertex_shader);
    glAttachShader(m_program, m_fragment_shader);
    glBindFragDataLocation(m_program, 0, "out_color");
    glLinkProgram(m_program);
    glDetachShader(m_program, m_fragment_shader);
    glDetachShader(m_program, m_vertex_shader);
    glUseProgram(m_program);

    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(m_program, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
}

string Renderer::check_shader() {
    GLint success = 0;
    glGetShaderiv(m_fragment_shader, GL_COMPILE_STATUS, &success);
    GLint max_length = 0;
    glGetShaderiv(m_fragment_shader, GL_INFO_LOG_LENGTH, &max_length);
    if (!success) {
        vector<GLchar> error_log(max_length);
        glGetShaderInfoLog(m_fragment_shader, max_length, &max_length, &error_log[0]);
        return &error_log[0];
    }
    return "";
}

string Renderer::load_renderer(int msaa_level, resolution res) {
    if (!glfwInit()) {
        return "Failed to initialize GLFW";
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, msaa_level);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    m_window = glfwCreateWindow((int)res.x, (int)res.y, ".: soup :.", NULL, NULL);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    glfwSetErrorCallback(error_callback);
    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, 1);
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    if (gl3wInit()) {
        return "Failed to initialize OpenGL";
    }
    return "";
}

resolution Renderer::get_resolution() {
    int width;
    int height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return resolution {
        .x = (float)width,
        .y = (float)height
    };
}

GLint Renderer::get_uniform_location(const char* name) {
    return glGetUniformLocation(m_program, name);
}

GLFWwindow* Renderer::get_window() {
    return m_window;
}

void Renderer::print_renderer() {
    const GLubyte* render = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    string log_render = string("Renderer: ").append((char*)render);
    string log_version = string("OpenGL version supported: ").append((char*)version);
    cout << log_render << endl;
    cout << log_version << endl;
}

void Renderer::draw_buffer() {
    glClearColor(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    if(m_draw_fps) {
        draw_fps();
    }

    glfwSwapBuffers(m_window);
}

void Renderer::enable_fps() {
    m_draw_fps = true;
    t0 = glfwGetTime();
}

void Renderer::draw_fps() {
    t = glfwGetTime();

    if((t - t0) > 1.0 || m_fps == 0) {
        m_fps = m_fps / (t - t0);
        string fps_str = to_string(round(m_fps * 10.0) / 10.0);
        fps_str.erase(fps_str.find_last_not_of('0') + 1, string::npos);
        if(fps_str[fps_str.length() - 1] == '.') {
            fps_str.append("0");
        }
        glfwSetWindowTitle(m_window, string(".: soup :. ~ ").append(fps_str).c_str());
        t0 = t;
        m_fps = 0;
    }
    m_fps++;
}
