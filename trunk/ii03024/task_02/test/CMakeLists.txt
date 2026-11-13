include(FetchContent)
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()
FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Тесты линкуются с core_ii03024: туда входят модели ЛР №1 (task_01), поэтому код не дублируется.
add_executable(task2_tests
    test_models.cpp
    test_stability.cpp
    test_signals.cpp
)
target_link_libraries(task2_tests PRIVATE core_ii03024 GTest::gtest_main GTest::gmock)

include(GoogleTest)
gtest_discover_tests(task2_tests)
