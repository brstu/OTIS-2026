include(FetchContent)

# Google Test / Google Mock скачиваются при первой конфигурации
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)   # для MSVC
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(task_02_tests
    test_input_signals.cpp
    test_models.cpp
    test_simulation.cpp
    test_stability.cpp
)

target_link_libraries(task_02_tests PRIVATE
    lab_core
    gtest_main
    gmock
)

include(GoogleTest)
gtest_discover_tests(task_02_tests DISCOVERY_MODE PRE_TEST)
