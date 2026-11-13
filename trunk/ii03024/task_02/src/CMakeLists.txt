# Модели берутся из ЛР №1 (дублирования кода нет): подключаются напрямую из task_01.
# В репозитории каталог ЛР №1 называется "scr"; если его переименуют в "src" - тоже сработает.
set(TASK01_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../task_01/src)
if(NOT EXISTS ${TASK01_DIR}/model11.cpp)
    set(TASK01_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../task_01/scr)
endif()
set(TASK01_DIR ${TASK01_DIR} CACHE INTERNAL "Каталог с исходниками ЛР №1")

# Библиотека: модели ЛР №1 + новые модули ЛР №2 (анализ устойчивости, сигналы, симулятор)
add_library(core_ii03024 STATIC
    ${TASK01_DIR}/model11.cpp
    ${TASK01_DIR}/model28.cpp
    ${TASK01_DIR}/model36.cpp
    StabilityAnalyzer.cpp
    InputSignal.cpp
    Simulator.cpp
)
target_include_directories(core_ii03024 PUBLIC ${TASK01_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(core_ii03024 PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(core_ii03024 PUBLIC /utf-8)
endif()

add_executable(task2_ii03024 main.cpp)
target_link_libraries(task2_ii03024 PRIVATE core_ii03024)
