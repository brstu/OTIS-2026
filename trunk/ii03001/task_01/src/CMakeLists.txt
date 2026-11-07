cmake_minimum_required(VERSION 3.16)
project(task_01_ii03001 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(task_01_ii03001 main.cpp)
set_target_properties(task_01_ii03001 PROPERTIES OUTPUT_NAME task_01)