cmake_minimum_required(VERSION 3.16)

project(Task01)

execute_process(
    COMMAND py -c "import sys; print(sys.executable)"
    OUTPUT_VARIABLE PYTHON_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(Python3_EXECUTABLE "${PYTHON_PATH}")

find_package(Python3 COMPONENTS Interpreter REQUIRED)

message(STATUS "Python: ${Python3_EXECUTABLE}")

# Преобразуем Windows-пути в формат с /
file(TO_CMAKE_PATH "${Python3_EXECUTABLE}" PYTHON_PATH)
file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/plot.py" PLOT_PATH)

message(STATUS "Python path: ${PYTHON_PATH}")
message(STATUS "Plot path: ${PLOT_PATH}")

add_executable(Task01
    main.cpp
)

target_compile_features(Task01 PRIVATE cxx_std_17)

target_compile_definitions(Task01 PRIVATE
    PYTHON_EXECUTABLE="${PYTHON_PATH}"
    PLOT_SCRIPT="${PLOT_PATH}"
)