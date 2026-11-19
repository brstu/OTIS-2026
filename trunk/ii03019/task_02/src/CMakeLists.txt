cmake_minimum_required(VERSION 3.15)
project(Task02_Tests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    FIND_PACKAGE_ARGS NAMES Gtest
)

# Для Windows, чтобы избежать конфликтов с CRT
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Флаги для покрытия кода (Gcov)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(--coverage -O0)
    add_link_options(--coverage)
endif()

# Подключаем исходники из ЛР1 (путь поднимается на 2 уровня вверх: из src в task_02, затем в ii03019)
include_directories(../../task_01/src)
file(GLOB TASK01_SOURCES "../../task_01/src/*.cpp")
# Исключаем main.cpp из ЛР1, чтобы он не конфликтовал с main() от Google Test
list(FILTER TASK01_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

# Явно указываем файл анализатора (он в этой же папке)
set(TASK02_CORE ${CMAKE_CURRENT_SOURCE_DIR}/StabilityAnalyzer.cpp)

# Создаем библиотеку ядра (исходники ЛР1 + наш модуль)
add_library(core STATIC ${TASK01_SOURCES} ${TASK02_CORE})

# Сама программа (модифицированный main)
add_executable(app main.cpp)
target_link_libraries(app core)

# Тесты (все файлы, начинающиеся на test_ в этой же папке)
file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp")
add_executable(test_runner ${TEST_SOURCES})
target_link_libraries(test_runner 
    core 
    GTest::gtest 
    GTest::gmock 
    GTest::gtest_main 
    pthread
)

enable_testing()

add_test(NAME RunAllTests COMMAND test_runner)