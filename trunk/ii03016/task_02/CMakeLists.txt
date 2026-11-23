cmake_minimum_required(VERSION 3.14)
project(Task02_Tests)
set(CMAKE_CXX_STANDARD 17)

# Подключаем пути к заголовочным файлам
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../task_01/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# 1. Заглядываем в папку src (там будет свой CMakeLists.txt)
add_subdirectory(src)

# 2. Подключаем Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 3. Собираем тесты из папки test
file(GLOB TEST_SOURCES "test/*.cpp")
add_executable(run_tests ${TEST_SOURCES})

# 4. Линкуем тесты с библиотекой из src и Google Test
# Обрати внимание: lab2_lib — это имя библиотеки, которую мы создадим в src
target_link_libraries(run_tests PRIVATE lab2_lib GTest::gtest_main)

# 5. Регистрируем тесты
enable_testing()
add_test(NAME AllTests COMMAND run_tests)