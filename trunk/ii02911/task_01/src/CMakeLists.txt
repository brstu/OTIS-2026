cmake_minimum_required(VERSION 3.15)

project(OTIS_Task_01_Variant_30 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(otis_task_01_ii02911
    main.cpp
    Model1_9.cpp
    Model2_6.cpp
    Model3_9.cpp
    InputSignal.cpp
)

target_include_directories(otis_task_01 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

configure_file(plot.py plot.py COPYONLY)
