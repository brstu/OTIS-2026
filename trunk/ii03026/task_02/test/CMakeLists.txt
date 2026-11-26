cmake_minimum_required(VERSION 3.16)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(run_unit_tests
        test_boundary.cpp
        test_models.cpp
        test_simulation.cpp
        test_stability.cpp
)

target_include_directories(run_unit_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(run_unit_tests PRIVATE
        task2_core
        GTest::gtest_main
        GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(run_unit_tests)