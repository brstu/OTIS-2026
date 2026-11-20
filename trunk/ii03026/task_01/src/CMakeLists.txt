cmake_minimum_required(VERSION 3.16)

project(ii03026_task_01 LANGUAGES CXX)

add_executable(ii03026_task_01
        main.cpp
        Models.cpp
        Utils.cpp
)

target_include_directories(ii03026_task_01 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(ii03026_task_01 PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)