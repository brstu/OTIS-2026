
cmake_minimum_required(VERSION 3.14)

project(Lab02Models LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lab02_ii02912
    main.cpp
    Model.hpp
    Input.hpp
    stabl.hpp
)

if(MSVC)
    target_compile_options(lab02_ii02912 PRIVATE /W4 /utf-8)
else()
    target_compile_options(lab02_ii02912 PRIVATE -Wall -Wextra)
endif()

set_target_properties(lab02_ii02912 PROPERTIES OUTPUT_NAME lab01)
