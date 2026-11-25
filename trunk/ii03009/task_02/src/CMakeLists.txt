add_library(ModelLibrary INTERFACE)

target_sources(ModelLibrary INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}/Model_1_8.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/InputSignal.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/StabilityAnalyzer.h"
)

target_include_directories(ModelLibrary INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})