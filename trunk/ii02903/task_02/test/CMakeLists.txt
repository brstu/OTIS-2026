# Google Test + Google Mock are downloaded by CMake (the first run needs the internet).
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)   # same runtime library as the project (Visual Studio)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(task02_tests
    mocks.h
    test_signals.cpp
    test_linear_model13.cpp
    test_nonlinear_model25.cpp
    test_diff_model37.cpp
    test_stability.cpp
    test_simulation.cpp)
target_link_libraries(task02_tests PRIVATE task02_core GTest::gtest_main GTest::gmock)

# every TEST / TEST_P becomes a separate ctest test
include(GoogleTest)
gtest_discover_tests(task02_tests DISCOVERY_TIMEOUT 60)
