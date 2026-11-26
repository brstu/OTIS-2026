cmake_minimum_required(VERSION 3.10)
project(ii03023_task01 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ii03023_lab1 main.cpp models.h)

if(MSVC)
    target_compile_options(task_01_ii03021 PRIVATE /utf-8)
endif()