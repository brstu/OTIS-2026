# Ядро: вся логика в статической библиотеке, чтобы её могли линковать тесты.
add_library(ii02926_task_02_core STATIC
    StabilityAnalyzer.cpp
    Simulation.cpp
    Report.cpp
    ConsoleInput.cpp
    Application.cpp
)
target_include_directories(ii02926_task_02_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(ii02926_task_02_core PUBLIC cxx_std_17)

add_executable(ii02926_task_02 main.cpp)
target_link_libraries(ii02926_task_02 PRIVATE ii02926_task_02_core)
