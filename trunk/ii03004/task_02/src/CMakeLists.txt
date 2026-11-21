add_library(app_lib
    StabilityAnalyzer.cpp
    StabilityAnalyzer.h
    ApplicationLogic.h
)

target_include_directories(app_lib PUBLIC .)