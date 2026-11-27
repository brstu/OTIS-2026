add_executable(unit_tests
    test_stability.cpp
    test_simulation.cpp
)

target_link_libraries(unit_tests PRIVATE
    app_logic
    GTest::gtest_main
    GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)