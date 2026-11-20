cmake_minimum_required(VERSION 3.14)
project(Task02_Tests)
set(CMAKE_CXX_STANDARD 17)

# ВОТ ЭТИ ДВЕ СТРОЧКИ ДОЛЖНЫ БЫТЬ:
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../task_01/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB LR2_SOURCES "src/*.cpp")
add_library(lab2_lib STATIC ${LR2_SOURCES})

enable_testing()
add_subdirectory(test)