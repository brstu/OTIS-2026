cmake_minimum_required(VERSION 3.10)

# Имя программы берём из названий папок (например ii02999_task_01), чтобы оно
# не совпало с программами других студентов, когда собирается весь репозиторий.
string(REGEX MATCH "([^/]+)/([^/]+)/src$" path_match "${CMAKE_CURRENT_SOURCE_DIR}")
if(path_match)
    set(TARGET_NAME "${CMAKE_MATCH_1}_${CMAKE_MATCH_2}")
else()
    set(TARGET_NAME "task_01")
endif()

project(${TARGET_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${TARGET_NAME} main.cpp models.h)

# Файл с исходниками в кодировке UTF-8 (чтобы русский текст не ломался в MSVC)
if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /utf-8)
endif()

# Какое бы длинное имя ни было у цели, сам файл называется task_01(.exe)
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "task_01")
