add_executable(run_unit_tests
    test_stability.cpp
    test_models_and_params.cpp
    test_signals_mock.cpp
)

target_link_libraries(run_unit_tests
    PRIVATE
        otIS_core
        GTest::gtest_main
        GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(run_unit_tests)