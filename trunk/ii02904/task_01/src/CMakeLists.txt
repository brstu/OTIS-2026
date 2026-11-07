cmake_minimum_required(VERSION 3.10)

string(REGEX MATCH "([^/]+)/([^/]+)/src$" path_match "${CMAKE_CURRENT_SOURCE_DIR}")
if(path_match)
    set(TARGET_NAME "${CMAKE_MATCH_1}_${CMAKE_MATCH_2}")
else()
    set(TARGET_NAME "task_01")
endif()

project(${TARGET_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${TARGET_NAME} main.cpp models.h)

if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /utf-8)
endif()

set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "task_01")
