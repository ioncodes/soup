cmake_minimum_required(VERSION 3.10)
project(soup)

set(CMAKE_CXX_STANDARD 17)

find_library (OpenGL REQUIRED)

add_executable(soup gl3w.c main.cpp renderer.cpp renderer.h project.cpp project.h tinyxml2.cpp tinyxml2.h resolution.h cxxopts.hpp uniform.h)

target_link_libraries(soup glfw bass luajit-5.1 "-pagezero_size 10000 -image_base 100000000")