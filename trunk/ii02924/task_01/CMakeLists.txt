cmake_minimum_required(VERSION 3.15)
project(task_01_ii02924 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
    src/Model.h
    src/Model1_8.h
    src/Model2_3.h
    src/Model3_3.h
)

add_executable(task_01 ${SOURCES})

# Автоматическое копирование plot.py в папку сборки рядом с .exe файлом
add_custom_command(
    TARGET task_01 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/src/plot.py"
    "$<TARGET_FILE_DIR:task_01>/plot.py"
)