cmake_minimum_required(VERSION 3.19)
project(img2cpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(img2cpp main.cpp stb_image.cpp)