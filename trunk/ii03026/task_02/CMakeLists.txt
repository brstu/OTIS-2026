cmake_minimum_required(VERSION 3.16)
project(ii03026_task_02)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(task2_core src/Models.cpp)
target_include_directories(task2_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(ii03026_task_02 src/main.cpp)
target_link_libraries(ii03026_task_02 PRIVATE task2_core)

add_subdirectory(test)