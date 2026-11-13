# Core library: everything except main(), so that unit tests can link against it.
add_library(otis_core STATIC
    models.cpp
    signals.cpp
    simulator.cpp
    stability.cpp
)
target_include_directories(otis_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(otis_core PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(otis_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(otis_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Code coverage (gcov/gcovr): -O0 so that line counts are exact.
if(OTIS_COVERAGE AND NOT MSVC)
    target_compile_options(otis_core PUBLIC --coverage -O0 -g)
    target_link_options(otis_core PUBLIC --coverage)
endif()

add_executable(otis_variant2 main.cpp)
target_link_libraries(otis_variant2 PRIVATE otis_core)
if(MSVC)
    target_compile_options(otis_variant2 PRIVATE /W4 /permissive-)
else()
    target_compile_options(otis_variant2 PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Make the plotting script available beside the executable.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/plot.py
               ${CMAKE_CURRENT_BINARY_DIR}/plot.py COPYONLY)
add_custom_command(TARGET otis_variant2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/plot.py
            $<TARGET_FILE_DIR:otis_variant2>/plot.py
    COMMENT "Copying plot.py next to the executable")
