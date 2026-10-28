add_executable(lab01
    main.cpp
    Signals.h
    Model.h
    Model1_5.h
    Model2_9.h
    Model3_3.h
)

target_include_directories(lab01 PRIVAT
    ${CMAKE_CURRENT_SOURCE_DIR}
)