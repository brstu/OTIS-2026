add_executable(otis_lab1_ii02911
    main.cpp
    InputSignal.cpp
    Model1_1.cpp
    Model2_5.cpp
    Model3_9.cpp
    Simulator.cpp
    StabilityAnalyzer.cpp
)

set_target_properties(otis_lab1_ii02911 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

if(MSVC)
    target_compile_options(otis_lab1_ii02911 PRIVATE /W4 /utf-8)
else()
    target_compile_options(otis_lab1_ii02911 PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Единый каталог с exe: <build>/bin (в т.ч. для многоконфигурационного Visual Studio).
set_target_properties(otis_lab1_ii02911 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
foreach(config ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER "${config}" CONFIG_UPPER)
    set_target_properties(otis_lab1_ii02911 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${CMAKE_BINARY_DIR}/bin"
    )
endforeach()
