add_library(app_logic STATIC
    Simulation.cpp
    StabilityAnalyzer.cpp
    ApplicationLogic.cpp
)

target_include_directories(app_logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(main_app main.cpp)
target_link_libraries(main_app PRIVATE app_logic)