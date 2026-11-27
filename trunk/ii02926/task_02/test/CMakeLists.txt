add_executable(ii02926_task_02_tests
    test_stability.cpp
    test_models.cpp
    test_simulation_param.cpp
    test_simulation_mock.cpp
    test_application.cpp
)
target_link_libraries(ii02926_task_02_tests PRIVATE ii02926_task_02_core GTest::gtest_main GTest::gmock)

include(GoogleTest)
gtest_discover_tests(ii02926_task_02_tests)
