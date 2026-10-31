cmake_minimum_required(VERSION 3.15)
project(lab1 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lab1
    src/main.cpp
    src/linear_model_1_9.cpp
    src/nonlinear_model_2_9.cpp
    src/diff_eq_3_9.cpp
)


add_custom_command(TARGET lab1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/plot.py
        ${CMAKE_CURRENT_BINARY_DIR}/plot.py
)