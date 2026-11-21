add_executable(otis_tests
    test_stability.cpp
    test_models.cpp
    test_simulation_param.cpp
    test_simulation_mock.cpp
    test_application.cpp
)
target_link_libraries(otis_tests PRIVATE otis_core GTest::gtest_main GTest::gmock)

include(GoogleTest)
gtest_discover_tests(otis_tests)
