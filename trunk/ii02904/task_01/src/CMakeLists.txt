cmake_minimum_required(VERSION 3.10)
project(ii02904_task_1 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# exe кладём в build/bin (и для MinGW, и для Visual Studio)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

add_executable(ii02904_task_1
    main.cpp
    Model.hpp
    Models.hpp
    Input.hpp
)
