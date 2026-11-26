
cmake_minimum_required(VERSION 3.14)

project(Lab02Tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "Tests must be built with GCC to measure coverage using gcov.")
endif()

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(stabltest
    stabltest.cpp
)

target_include_directories(stabltest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
)

target_link_libraries(stabltest PRIVATE
    GTest::gtest_main
    GTest::gmock
)

target_compile_options(stabltest PRIVATE
    -Wall
    -Wextra
    -O0
    -g
    --coverage
)

target_link_options(stabltest PRIVATE
    --coverage
)

include(GoogleTest)

gtest_discover_tests(stabltest)
