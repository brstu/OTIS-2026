add_library(ii02925_task_03_control STATIC control.cpp)
target_include_directories(ii02925_task_03_control PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(ii02925_task_03_control PUBLIC cxx_std_17)

add_executable(ii02925_task_03 main.cpp)
set_target_properties(ii02925_task_03 PROPERTIES OUTPUT_NAME lab3)
target_link_libraries(ii02925_task_03 PRIVATE ii02925_task_03_control)
target_compile_features(ii02925_task_03 PRIVATE cxx_std_17)

if(WIN32)
  target_link_libraries(ii02925_task_03 PRIVATE shell32)
endif()

if(MSVC)
  target_compile_options(ii02925_task_03_control PRIVATE /W4)
else()
  target_compile_options(ii02925_task_03_control PRIVATE -Wall -Wextra -Wpedantic)
endif()
