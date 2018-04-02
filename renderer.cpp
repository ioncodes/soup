//
// Created by Luca Marcelli on 02.04.18.
//

#include "renderer.h"

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
}
