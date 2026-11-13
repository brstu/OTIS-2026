cmake_minimum_required(VERSION 3.20)

project(task_02 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Покрытие кода (gcov/gcovr): включено по умолчанию для удобной генерации HTML
option(ENABLE_COVERAGE "Instrument the code for gcov/gcovr coverage" ON)

# ---------------------------------------------------------------------------
# Библиотека с логикой программы (моделей, сигналов, симуляции, анализа
# устойчивости). Её линкуют и основная программа, и модульные тесты.
# ---------------------------------------------------------------------------
add_library(lab_core STATIC
    src/Model1_10.cpp
    src/Model2_2.cpp
    src/Model3_4.cpp
    src/models.cpp
    src/InputSignals.cpp
    src/Simulation.cpp
    src/StabilityAnalyzer.cpp
)

target_include_directories(lab_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(MSVC)
    target_compile_options(lab_core PRIVATE /W4)
else()
    target_compile_options(lab_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(ENABLE_COVERAGE)
    if(MSVC)
        message(WARNING "ENABLE_COVERAGE requires GCC/Clang (MinGW); ignored for MSVC.")
    else()
        # -O0, чтобы строки не "схлопывались" оптимизатором
        target_compile_options(lab_core PUBLIC --coverage -O0 -g)
        target_link_options(lab_core PUBLIC --coverage)
    endif()
endif()

# ---------------------------------------------------------------------------
# Основная программа
# ---------------------------------------------------------------------------
add_executable(task_02 src/main.cpp)
target_link_libraries(task_02 PRIVATE lab_core)

if(MSVC)
    target_compile_options(task_02 PRIVATE /W4)
else()
    target_compile_options(task_02 PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ---------------------------------------------------------------------------
# Модульные тесты (Google Test + Google Mock)
# ---------------------------------------------------------------------------
enable_testing()
add_subdirectory(test)