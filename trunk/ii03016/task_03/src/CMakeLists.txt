cmake_minimum_required(VERSION 3.15)


project(task_03_ii03016)
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../task_01/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME}
    main.cpp
    PID.cpp
    Simulation.cpp
    Models.cpp
)
