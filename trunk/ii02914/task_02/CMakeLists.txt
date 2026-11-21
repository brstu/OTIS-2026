cmake_minimum_required(VERSION 3.16)
project(ii02914_task_02 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(OTIS_BUILD_TESTS "Build Google Test unit tests" ON)
option(OTIS_ENABLE_COVERAGE "GCC/Clang coverage instrumentation (gcov)" OFF)

add_subdirectory(src)

if(OTIS_ENABLE_COVERAGE)
    # Инструментируем только свой код (библиотеку и тесты), не GoogleTest.
    target_compile_options(otis_core PUBLIC --coverage -O0 -g)
    target_link_options(otis_core PUBLIC --coverage)
endif()

if(OTIS_BUILD_TESTS)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
        GIT_SHALLOW TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(test)
endif()
