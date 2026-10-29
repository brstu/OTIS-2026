cmake_minimum_required(VERSION 3.15)
project(task_01_ii02924 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main.cpp
    Model.h
    Model1_8.h
    Model2_3.h
    Model3_3.h
)

add_executable(task_01_ii02924 ${SOURCES})

add_custom_command(
    TARGET task_01_ii02924 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/plot.py"
    "$<TARGET_FILE_DIR:task_01_ii02924>/plot.py"
)