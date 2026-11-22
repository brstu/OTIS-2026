cmake_minimum_required(VERSION 3.21)
project(ii02904_task_02 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)

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

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ii02904_task_02_models PRIVATE -Wall -Wextra -Wpedantic)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(ii02904_task_02_models PRIVATE --coverage)
        target_link_options(ii02904_task_02_models INTERFACE --coverage)
    endif()
endif()

add_executable(task_02_tests test/test_models.cpp)
target_link_libraries(task_02_tests PRIVATE
    ii02904_task_02_models
    GTest::gtest_main
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(task_02_tests PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    target_compile_options(task_02_tests PRIVATE /W4 /utf-8)
endif()

include(GoogleTest)
gtest_discover_tests(task_02_tests)
