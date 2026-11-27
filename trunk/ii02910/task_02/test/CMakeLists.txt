include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(run_tests
    test_main.cpp
    test_linear_arx.cpp
    test_saturation.cpp
    test_cubic_ode.cpp
    test_stability.cpp
    test_param_signals.cpp 
    test_mock_signals.cpp
    test_input_generator.cpp
    test_system_factory.cpp)

target_link_libraries(run_tests PRIVATE
    ii02910_models
    gtest
    gtest_main
)

#include(GoogleTest)
#gtest_discover_tests(run_tests)