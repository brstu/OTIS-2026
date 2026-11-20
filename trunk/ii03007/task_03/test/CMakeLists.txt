include(FetchContent)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(task3_ii03007_tests
    test_regulator.cpp
)
target_include_directories(task3_ii03007_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(task3_ii03007_tests PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(task3_ii03007_tests DISCOVERY_TIMEOUT 30)