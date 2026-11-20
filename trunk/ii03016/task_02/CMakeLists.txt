cmake_minimum_required(VERSION 3.14)
project(Task02_Tests)
set(CMAKE_CXX_STANDARD 17)

# Указываем пути к папкам с кодом
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../task_01/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Собираем файлы из src напрямую (без add_subdirectory)
file(GLOB LR2_SOURCES "src/*.cpp")
add_library(lab2_lib STATIC ${LR2_SOURCES})

# Подключаем папку с тестами
enable_testing()
add_subdirectory(test)