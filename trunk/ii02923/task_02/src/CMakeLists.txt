add_library(otIS_core STATIC
    Model1_6.cpp
    Model2_1.cpp
    Model3_7.cpp
    InputSignal.cpp
    StabilityAnalyzer.cpp
    main.cpp
)
target_include_directories(otIS_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(otIS_core PRIVATE UNIT_TEST)

add_executable(task_02 main.cpp)
target_link_libraries(task_02 PRIVATE otIS_core)