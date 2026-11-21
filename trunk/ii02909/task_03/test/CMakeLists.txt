add_executable(run_tests_03
    test_pid.cpp
    test_plant.cpp
    ../src/PIDController.cpp
)

target_link_libraries(run_tests_03
    PRIVATE
        GTest::gtest_main
)

target_include_directories(run_tests_03
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)