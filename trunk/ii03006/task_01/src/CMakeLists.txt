cmake_minimum_required(VERSION 3.16)

add_executable(ii03006_task_01
    main.cpp
    InputSignal.cpp
    Model1_8.cpp
    Model2_3.cpp
    Model3_3.cpp
)

target_include_directories(ii03006_task_01 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(ii03006_task_01 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)