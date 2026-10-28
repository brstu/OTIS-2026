cmake_minimum_required(VERSION 3.15)
project(ii02927_task_01 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ii02927_task_01
    main.cpp
    DynamicModel.h
    Model1_6.h
    Model2_8.h
    Model3_10.h
    SignalGenerator.h
    ModelCreator.h
)