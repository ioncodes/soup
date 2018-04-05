//
// Created by Luca Marcelli on 05.04.18.
//

#ifndef SOUP_PROJECT_H
#define SOUP_PROJECT_H

#include "tinyxml2.h"
#include "resolution.h"
#include <string>
#include <iostream>

using namespace std;

typedef std::shared_ptr<tinyxml2::XMLDocument> XMLDocument;

class Project {
private:
    XMLDocument m_project;
    string m_project_name;
    resolution m_resolution;
    int m_msaa_level;
    string m_shader;
    float m_time_step;
    bool m_show_fps;
    string m_sound;

public:
    void load_project(string file);
    string project_name();
    int msaa_level();
    resolution screen_resolution();
    string shader();
    float time_step();
    bool show_fps();
    string sound();
    Project();
};


#endif //SOUP_PROJECT_H
