cmake_minimum_required(VERSION 3.16)
project(ii02919_task_02 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
include(FetchContent)

option(ENABLE_COVERAGE "Enable GCC/Clang coverage instrumentation" OFF)

if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

set(MODEL_SOURCES
    src/MultiStepModel.cpp
    src/QuadraticFeedback.cpp
    src/HarmonicODE.cpp
    src/InputSignal.cpp
    src/ProcessFactory.cpp
    src/StabilityAnalyzer.cpp
)

add_library(model_core ${MODEL_SOURCES})
target_include_directories(model_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(ii02919_task_02 src/main.cpp)
target_link_libraries(ii02919_task_02 PRIVATE model_core)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(unit_tests
    test/ModelTests.cpp
    test/StabilityTests.cpp
    test/ParameterizedTests.cpp
)
target_link_libraries(unit_tests PRIVATE model_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)
