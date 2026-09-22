cmake_minimum_required(VERSION 3.10)

project(ii02919_task_01_src LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ii02919_task_01
    main.cpp
    MultiStepModel.cpp
    QuadraticFeedback.cpp
    HarmonicODE.cpp
    InputSignal.cpp
    ProcessFactory.cpp
)
