#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>

GLuint m_program;
GLuint m_fragment_shader;
GLuint m_vertex_shader;

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error %d: %s\n", error, description);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    printf("Resizing to %dx%d\n", width, height);
    glViewport(0, 0, width, height);
}

void compile(char* file) {


    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(m_program, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

    GLint success = 0;
    glGetShaderiv(m_fragment_shader, GL_COMPILE_STATUS, &success);
    GLint maxLength = 0;
    glGetShaderiv(m_fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);
    if (!success) {
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(m_fragment_shader, maxLength, &maxLength, &errorLog[0]);
        std::cout << &errorLog[0] << std::endl;
    }
}

bool initGlew() {
    if (gl3wInit()) {
        fprintf(stderr, "Failed to initialize OpenGL\n");
        return false;
    }
    return true;
}

int main(int argc, char** argv) {
    if (!glfwInit())
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(1280, 720, ".: soup :.", NULL, NULL);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetErrorCallback(error_callback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if(!initGlew()) return -1;

    int width;
    int height;
    glfwGetFramebufferSize(window, &width, &height);

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    std::string log_render = std::string("Renderer: ").append((char*)renderer);
    std::string log_version = std::string("OpenGL version supported: ").append((char*)version);
    std::cout << log_render << std::endl;
    std::cout << log_version << std::endl;

    compile(argv[1]);

    GLint resolution_loc = glGetUniformLocation(m_program, "resolution");
    GLint time_loc = glGetUniformLocation(m_program, "time");
    float time = 0.00;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glUniform2f(resolution_loc, width, height);
        glUniform1f(time_loc, time);

        glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);

        time += 0.01;
    }

    glfwTerminate();

    glDeleteShader(m_fragment_shader);
    glDeleteShader(m_vertex_shader);
    glDeleteProgram(m_program);
}