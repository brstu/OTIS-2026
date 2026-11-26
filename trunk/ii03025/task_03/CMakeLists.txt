cmake_minimum_required(VERSION 3.14)

project(OTIS_Lab3_PID)

set(TASK01_SRC "../../task_01/src")

add_executable(OTIS_Lab3
    src/main.cpp
    src/pid.cpp
    "${TASK01_SRC}/model19.cpp"
    "${TASK01_SRC}/model23.cpp"
    "${TASK01_SRC}/model37.cpp"
)

include(FetchContent)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(tests
    test/test_pid.cpp
    src/pid.cpp
)

target_include_directories(tests PRIVATE src)

target_link_libraries(tests
    PRIVATE
    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(tests)
