include(GoogleTest)

add_executable(unit_tests
    test_models.cpp
    test_stability.cpp
    test_signals.cpp
)

target_link_libraries(unit_tests PRIVATE GTest::gtest_main ModelLibrary)
gtest_discover_tests(unit_tests)