cmake_minimum_required(VERSION 3.15)
project(task_01_ii02913 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_SOURCES
    main.cpp
    Model.h
    Model1_3.h
    Model2_7.h
    Model3_1.h
)

add_executable(task_01_ii02913 ${PROJECT_SOURCES})

add_custom_command(
    TARGET task_01_ii02913 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/plot.py"
    "$<TARGET_FILE_DIR:task_01_ii02913>/plot.py"
)