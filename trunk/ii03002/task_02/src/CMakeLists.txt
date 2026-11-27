# Reuse the implementation from task_01; do not copy model sources into task_02.
set(TASK01_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../task_01/src")

add_library(ii03002_task01_models STATIC
    "${TASK01_DIR}/InputSignal.cpp"
    "${TASK01_DIR}/Model1_9.cpp"
    "${TASK01_DIR}/Model2_1.cpp"
    "${TASK01_DIR}/Model3_3.cpp"
)
target_include_directories(ii03002_task01_models PUBLIC "${TASK01_DIR}")

add_executable(ii03002_task_02 main.cpp)
target_link_libraries(ii03002_task_02 PRIVATE ii03002_task01_models)
target_include_directories(ii03002_task_02 PRIVATE "${TASK01_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(ii03002_task01_models PRIVATE -O0 -g --coverage)
    target_link_options(ii03002_task01_models PRIVATE --coverage)
    target_compile_options(ii03002_task_02 PRIVATE -O0 -g --coverage)
    target_link_options(ii03002_task_02 PRIVATE --coverage)
endif()
