cmake_minimum_required(VERSION 3.10)
project(Lab1 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)


add_executable(lab1
    src/main.cpp
    src/Model.hpp
    src/Models.hpp
    src/Input.hpp
)
