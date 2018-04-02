cmake_minimum_required(VERSION 3.10)
project(soup)

set(CMAKE_CXX_STANDARD 17)

find_library (OpenGL REQUIRED)

add_executable(soup gl3w.c main.cpp renderer.cpp renderer.h)

target_link_libraries(soup glfw)