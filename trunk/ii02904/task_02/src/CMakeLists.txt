cmake_minimum_required(VERSION 3.10)

string(REGEX MATCH "([^/]+)/([^/]+)/src$" path_match "${CMAKE_CURRENT_SOURCE_DIR}")
if(path_match)
    set(TARGET_NAME "${CMAKE_MATCH_1}_${CMAKE_MATCH_2}")
else()
    set(TARGET_NAME "task_02")
endif()

project(${TARGET_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(${TARGET_NAME}_models
    models.cpp
    stability_analyzer.cpp
    input_signal.cpp
)
target_include_directories(${TARGET_NAME}_models PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE ${TARGET_NAME}_models)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${TARGET_NAME}_models PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    target_compile_options(${TARGET_NAME}_models PRIVATE /W4 /utf-8)
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /utf-8)
endif()
