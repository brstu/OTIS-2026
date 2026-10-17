cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(task_01
    main.cpp
    Model1_10.cpp
    Model2_2.cpp
    Model3_4.cpp
    InputSignals.cpp
)

if(MSVC)
    target_compile_options(task_01 PRIVATE /W4)
else()
    target_compile_options(task_01 PRIVATE -Wall -Wextra -Wpedantic)
endif()
