cmake_minimum_required(VERSION 3.10)
project(lab01_ii02915)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lab01_ii02915
    main.cpp
    Model.h
    Model1_5.h
    Model2_9.h
    Model3_3.h
    Signals.h
)

target_include_directories(lab01_ii02915 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

его запустить
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/plot.py
    ${CMAKE_CURRENT_BINARY_DIR}/plot.py
    COPYONLY
)