cmake_minimum_required(VERSION 3.21)
project(ii02904_task_02 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
include(FetchContent)

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(googletest)

add_library(task_models
    src/models.cpp
    src/stability_analyzer.cpp
    src/input_signal.cpp
)
target_include_directories(task_models PUBLIC src)

add_executable(task_02 src/main.cpp)
target_link_libraries(task_02 PRIVATE task_models)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(task_models PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(task_02 PRIVATE -Wall -Wextra -Wpedantic)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(task_models PRIVATE --coverage)
        target_link_options(task_models INTERFACE --coverage)
    endif()
elseif(MSVC)
    target_compile_options(task_models PRIVATE /W4 /utf-8)
    target_compile_options(task_02 PRIVATE /W4 /utf-8)
endif()

add_executable(task_02_tests test/test_models.cpp)
target_link_libraries(task_02_tests PRIVATE
    task_models
    GTest::gtest_main
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(task_02_tests PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    target_compile_options(task_02_tests PRIVATE /W4 /utf-8)
endif()

include(GoogleTest)
gtest_discover_tests(task_02_tests)
