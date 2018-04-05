#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include <BASS/bass.h>
#include <numeric>
#include "renderer.h"
#include "project.h"
#include "cxxopts.hpp"

Renderer renderer;
Project project;

int main(int argc, const char** argv) {
    renderer = Renderer();
    project = Project();
    BASS_Init(-1, 44100, 0, 0, NULL);

    cxxopts::Options options("soup", "Haven't you been waiting for this engine?");
    options.add_options()
            ("p,project", "Project file", cxxopts::value<string>())
            ;
    auto args = options.parse(argc, argv);

    string file = args["project"].as<string>();
    project.load_project(file);

    string project_name = project.project_name();
    resolution resolution = project.screen_resolution();
    int msaa_level = project.msaa_level();
    string shader = project.shader();
    float time_step = project.time_step();
    bool show_fps = project.show_fps();
    string sound = project.sound();

    cout << "Loaded project " << project_name << endl;
    cout << "Resolution: " << resolution.x << " x " << resolution.y << endl;
    cout << "MSAA: " << msaa_level << endl;
    cout << "Show FPS: " << show_fps << endl;
    cout << "Shader: " << shader << endl;
    cout << "Time Step: " << time_step << endl;
    cout << "Sound: " << sound << endl;

    string log = renderer.load_renderer(msaa_level, resolution);
    if(log.length() > 0) {
        cerr << log << endl;
    }

    resolution = renderer.get_resolution();

    renderer.print_renderer();

    renderer.compile_shader(shader.data());

    string shader_log = renderer.check_shader();
    if(shader_log.length() > 0) {
        cerr << shader_log << endl;
    }

    GLint resolution_loc = renderer.get_uniform_location("resolution");
    GLint time_loc = renderer.get_uniform_location("time");
    GLint spectrum_loc = renderer.get_uniform_location("spectrum");
    float time = 0.00;

    auto window = renderer.get_window();

    glUniform2f(resolution_loc, resolution.x, resolution.y);

    if(show_fps) {
        renderer.enable_fps();
    }

    auto stream = BASS_StreamCreateFile(FALSE, sound.c_str(), 0, 0, BASS_SAMPLE_LOOP);
    BASS_ChannelPlay(stream, FALSE);

    double mag;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glUniform1f(time_loc, time);
        glUniform1f(spectrum_loc, (float)mag);

        float _fft[1024];
        BASS_ChannelGetData(stream, _fft, BASS_DATA_FFT2048);

        std::vector<float> fft(_fft, _fft + sizeof _fft / sizeof _fft[0]);

        mag = std::accumulate(fft.begin(), fft.end(),
                                     0.0);
        // double db = 20 * log10(mag);

        renderer.draw_buffer();

        time += time_step;
    }

    glfwTerminate();
    BASS_Free();

    cout << "Bye." << endl;
}