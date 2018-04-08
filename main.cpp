#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include <BASS/bass.h>
#include <lua/lua.hpp>
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
    lua_State* vm = luaL_newstate();
    luaL_openlibs(vm);

    cxxopts::Options options("soup", "Haven't you been waiting for this engine?");
    options.add_options()
            ("p,project", "Project file", cxxopts::value<string>()->default_value(""))
            ("s,shader", "Shader file", cxxopts::value<string>()->default_value(""))
            ("x,resx", "Width", cxxopts::value<int>()->default_value("1080"))
            ("y,resy", "Height", cxxopts::value<int>()->default_value("720"))
            ("m,msaa", "MSAA level", cxxopts::value<int>()->default_value("0"))
            ("l,lua", "Lua script", cxxopts::value<string>()->default_value(""))
            ("t,time", "Time step", cxxopts::value<float>()->default_value("0.01"))
            ("f,fps", "Show fps", cxxopts::value<bool>()->default_value("false"))
            ("a,audio", "Audio file", cxxopts::value<string>()->default_value(""))
            ("h,help", "Show help")
            ;
    auto args = options.parse(argc, argv);

    if(args["help"].as<bool>()) {
        cout << options.help() << endl;
        return 0;
    }

    string shader;
    resolution res;
    string project_name;
    int msaa_level;
    float time_step;
    bool show_fps;
    string sound;
    string script;
    vector<uniform> uniforms;

    string file = args["project"].as<string>();
    bool not_project = false;
    if(file.empty()) {
        file = args["shader"].as<string>();
        res = resolution {
            .x = (float)args["resx"].as<int>(), .y = (float)args["resy"].as<int>()
        };
        msaa_level = args["msaa"].as<int>();
        script = args["lua"].as<string>();
        sound = args["audio"].as<string>();
        time_step = args["time"].as<float>();
        show_fps = args["fps"].as<bool>();
        not_project = true;
    }

    if(!not_project) {
        project.load_project(file);
        project_name = project.project_name();
        res = project.screen_resolution();
        msaa_level = project.msaa_level();
        shader = project.shader();
        time_step = project.time_step();
        show_fps = project.show_fps();
        sound = project.sound();
        script = project.script();
        uniforms = project.uniforms();
    } else {
        shader = file;
    }

    if(!not_project) {
        cout << "Loaded project " << project_name << endl;
    }
    cout << "Resolution: " << res.x << " x " << res.y << endl;
    cout << "MSAA: " << msaa_level << endl;
    cout << "Show FPS: " << show_fps << endl;
    cout << "Time Step: " << time_step << endl;
    cout << "Sound: " << sound << endl;
    cout << "Script: " << script << endl;
    cout << "Shader: " << shader << endl;

    if(!script.empty()) {
        if (luaL_loadfile(vm, script.c_str()) || lua_pcall(vm, 0, 0, 0)) {
            cerr << "error loading " << script << endl;
            vm = nullptr;
        }
    } else {
        vm = nullptr;
    }

    string log = renderer.load_renderer(msaa_level, res);
    if(log.length() > 0) {
        cerr << log << endl;
    }

    res = renderer.get_resolution();

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

    glUniform2f(resolution_loc, res.x, res.y);

    if(show_fps) {
        renderer.enable_fps();
    }

    HSTREAM stream;
    if(!sound.empty()) {
        stream = BASS_StreamCreateFile(FALSE, sound.c_str(), 0, 0, BASS_SAMPLE_LOOP);
        BASS_ChannelPlay(stream, FALSE);
    }

    double mag;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glUniform1f(time_loc, time);
        glUniform1f(spectrum_loc, (float) mag);

        renderer.draw_buffer();

        if(!sound.empty()) {
            float _fft[1024];
            BASS_ChannelGetData(stream, _fft, BASS_DATA_FFT2048);
            std::vector<float> fft(_fft, _fft + sizeof _fft / sizeof _fft[0]);
            mag = std::accumulate(fft.begin(), fft.end(), 0.0);
        }

        time += time_step;

        if(vm != nullptr) {
            lua_pushstring(vm, "update");
            lua_gettable(vm, LUA_GLOBALSINDEX);
            lua_pcall(vm, 0, 0, 0);

            lua_pushnumber(vm, mag);
            lua_setglobal(vm, "spectrum");

            for (auto uniform : uniforms) {
                lua_getglobal(vm, uniform.name.c_str());
                auto value = (float) lua_tonumber(vm, -1);
                uniform.value = value;
                GLint loc = renderer.get_uniform_location(uniform.name.c_str());
                glUniform1f(loc, uniform.value);
            }
        }
    }

    glfwTerminate();
    if(!sound.empty()) {BASS_Free();}
    if(vm) lua_close(vm);

    cout << "Bye." << endl;
}