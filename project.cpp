//
// Created by Luca Marcelli on 05.04.18.
//

#include "project.h"

string get_project_dir(string file) {
#ifdef _WIN32
    string key = "\\";
#else
    string key = "/";
#endif
    size_t found = file.rfind(key);
    if(found != string::npos) {
        file.replace(found, file.length(), key);
    } else {
        file = "";
    }
    return file;
}


void Project::load_project(string file) {
    tinyxml2::XMLError result = m_project->LoadFile(file.c_str());
    if(result != tinyxml2::XML_SUCCESS) {
        cerr << result << endl;
    } else {
        m_project_name = m_project->FirstChildElement("PROJECT")->FirstChildElement("NAME")->GetText();
        m_msaa_level = atoi(m_project->FirstChildElement("PROJECT")->FirstChildElement("SETTINGS")->FirstChildElement("MSAA")->GetText());
        auto res_node = m_project->FirstChildElement("PROJECT")->FirstChildElement("SETTINGS")->FirstChildElement("RESOLUTION");
        m_resolution = resolution {
            .x = (float)atof(res_node->FirstChildElement("WIDTH")->GetText()),
            .y = (float)atof(res_node->FirstChildElement("HEIGHT")->GetText())
        };
        m_show_fps = (bool)m_project->FirstChildElement("PROJECT")->FirstChildElement("SETTINGS")->FirstChildElement("SHOW_FPS")->GetText();
        string shader_file = m_project->FirstChildElement("PROJECT")->FirstChildElement("SHADER")->GetText();
        string sound_file = m_project->FirstChildElement("PROJECT")->FirstChildElement("SOUND")->GetText();
        m_shader = get_project_dir(file).append(shader_file);
        m_sound = get_project_dir(file).append(sound_file);
        m_time_step = (float)atof(m_project->FirstChildElement("PROJECT")->FirstChildElement("STEP")->GetText());
    }
}

string Project::project_name() {
    return m_project_name;
}

Project::Project() {
    XMLDocument project(new tinyxml2::XMLDocument);
    m_project = project;
}

int Project::msaa_level() {
    return m_msaa_level;
}

resolution Project::screen_resolution() {
    return m_resolution;
}

string Project::shader() {
    return m_shader;
}

float Project::time_step() {
    return m_time_step;
}

bool Project::show_fps() {
    return m_show_fps;
}

string Project::sound() {
    return m_sound;
}
