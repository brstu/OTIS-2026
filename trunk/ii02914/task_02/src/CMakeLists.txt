# Ядро: вся логика в статической библиотеке, чтобы её могли линковать тесты.
add_library(otis_core STATIC
    StabilityAnalyzer.cpp
    Simulation.cpp
    Report.cpp
    ConsoleInput.cpp
    Application.cpp
)
target_include_directories(otis_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(otis_core PUBLIC cxx_std_17)

add_executable(ii02914_task_02 main.cpp)
target_link_libraries(ii02914_task_02 PRIVATE otis_core)
