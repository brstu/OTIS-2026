cmake_minimum_required(VERSION 4.5)
project(task_01 CXX)

# Требуем стандарт C++17 (обязательно для init-statement в if/switch)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Добавляем предупреждения для контроля качества кода
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Указываем исполняемый файл и все файлы исходного кода
add_executable(task_01
    src/main.cpp
    src/Model.h
    src/InputSignal.h
    src/InputSignal.cpp
    src/Model1_9.h
    src/Model1_9.cpp
    src/Model2_3.h
    src/Model2_3.cpp
    src/Model3_7.h
    src/Model3_7.cpp
)

# Указываем директорию для поиска заголовочных файлов
target_include_directories(task_01 PRIVATE src)