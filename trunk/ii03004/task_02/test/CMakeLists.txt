add_executable(unit_tests
    test_stability.cpp
)

target_link_libraries(unit_tests
    PRIVATE
    app_lib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)