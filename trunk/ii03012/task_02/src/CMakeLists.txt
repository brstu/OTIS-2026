add_library(ii03012_models STATIC Model.cpp LinearModels.cpp NonLinearModels.cpp
    DiffEquations.cpp InputSignal.cpp Simulator.cpp StabilityAnalyzer.cpp)
target_include_directories(ii03012_models PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(ii03012_models PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(ii03012_models PUBLIC /utf-8)
endif()
add_executable(ii03012_task_02 main.cpp)
target_link_libraries(ii03012_task_02 PRIVATE ii03012_models)
add_custom_command(TARGET ii03012_task_02 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/plot.py"
        "$<TARGET_FILE_DIR:ii03012_task_02>/plot.py" VERBATIM)
