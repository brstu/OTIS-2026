cmake_minimum_required(VERSION 3.10)
project(Lab01Models LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lab01 main.cpp Model.hpp Input.hpp)
if(MSVC)
  target_compile_options(lab01 PRIVATE /W4 /utf-8)
else()
  target_compile_options(lab01 PRIVATE -Wall -Wextra)
endif()
