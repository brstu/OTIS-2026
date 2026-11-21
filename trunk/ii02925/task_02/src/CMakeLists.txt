add_library(ii02925_task_02_models STATIC
  models.cpp
  stability.cpp
)
target_include_directories(ii02925_task_02_models PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(ii02925_task_02_models PUBLIC cxx_std_17)

add_executable(ii02925_task_02 main.cpp)
set_target_properties(ii02925_task_02 PROPERTIES OUTPUT_NAME lab2)
target_link_libraries(ii02925_task_02 PRIVATE ii02925_task_02_models)
target_compile_features(ii02925_task_02 PRIVATE cxx_std_17)

if(WIN32)
  target_link_libraries(ii02925_task_02 PRIVATE shell32)
endif()

if(MSVC)
  target_compile_options(ii02925_task_02_models PRIVATE /W4)
else()
  target_compile_options(ii02925_task_02_models PRIVATE -Wall -Wextra -Wpedantic)
endif()
