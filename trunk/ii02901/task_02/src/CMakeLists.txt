cmake_minimum_required(VERSION 3.14)
project(ii02901_task2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)

add_executable(run_tests 
    ../test/tests.cpp 
    model.cpp
)

target_link_libraries(run_tests gtest gmock gtest_main)

enable_testing()
add_test(NAME AllTests COMMAND run_tests)