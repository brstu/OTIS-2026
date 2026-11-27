cmake_minimum_required(VERSION 3.15)


project(Task03_PID)
set(CMAKE_CXX_STANDARD 17)

# Подключаем пути к заголовкам
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../task_01/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Собираем исполняемый файл
file(GLOB SOURCES "src/*.cpp")
add_executable(task_03_pid ${SOURCES})

# Подключаем Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Собираем тесты
file(GLOB TEST_SOURCES "test/*.cpp")
add_executable(run_tests ${TEST_SOURCES})
target_sources(run_tests PRIVATE src/PID.cpp)
target_link_libraries(run_tests PRIVATE GTest::gtest_main)

enable_testing()
add_test(NAME AllTests COMMAND run_tests)