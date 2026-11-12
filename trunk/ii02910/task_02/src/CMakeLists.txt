add_library(ii02910_models
    LinearARXModel.cpp
    SaturationModel.cpp
    CubicODE.cpp
    InputGenerator.cpp
    SystemFactory.cpp
)
target_include_directories(ii02910_models PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
