cmake_minimum_required(VERSION 3.14)
project(ii03017_task02 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем тестирование
enable_testing()

# Подключаем Google Test автоматически через FetchContent
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Для безопасности на Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Добавляем папки с исходным кодом и тестами
add_subdirectory(src)
add_subdirectory(test)