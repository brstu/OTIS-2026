cmake_minimum_required(VERSION 3.16)

add_library(task2_core STATIC
        Models.cpp
)

target_include_directories(task2_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(ii03026_task_02
        main.cpp
)

target_link_libraries(ii03026_task_02 PRIVATE
        task2_core
)