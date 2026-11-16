
cmake_minimum_required(VERSION 3.15)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

project(OTIS_Task02 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_library(models_lib
    src/StabilityAnalyzer.cpp
    ../task_01/src/Model1_9.cpp
    ../task_01/src/Model2_6.cpp
    ../task_01/src/Model3_9.cpp
    ../task_01/src/InputSignal.cpp
)

target_include_directories(models_lib
    PUBLIC
        src
        ../task_01/src
)

add_executable(unit_tests
    test/main_test.cpp
    test/TestModels.cpp
    test/TestStability.cpp
    test/TestNonLinear.cpp
)

target_include_directories(unit_tests
    PRIVATE
        src
        ../task_01/src
)

target_link_libraries(unit_tests
    PRIVATE
        models_lib
        gtest
        gtest_main
        gmock
)

# Настройка покрытия кода для GCC и Clang
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

    # Инструментирование исходников библиотеки
    target_compile_options(models_lib
        PRIVATE
            --coverage
            -O0
            -g
    )

    # Инструментирование тестов
    target_compile_options(unit_tests
        PRIVATE
            --coverage
            -O0
            -g
    )

    # Подключение библиотеки покрытия при компоновке
    target_link_options(unit_tests
        PRIVATE
            --coverage
    )

endif()

include(GoogleTest)

gtest_discover_tests(unit_tests)
