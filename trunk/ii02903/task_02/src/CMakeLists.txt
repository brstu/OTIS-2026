# Library with everything except main(): the program and the tests use the same code.
add_library(task02_core STATIC
    models.cpp
    models.h
    simulation.cpp
    simulation.h
    stability.cpp
    stability.h
    signals.h)
target_include_directories(task02_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(task02 main.cpp)
target_link_libraries(task02 PRIVATE task02_core)

# plot.py is copied next to the executable when CMake configures the project
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/plot.py" "${CMAKE_BINARY_DIR}/bin/plot.py" COPYONLY)
