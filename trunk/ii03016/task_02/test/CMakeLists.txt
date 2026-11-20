include(FetchContent)
FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Берем все файлы .cpp из папки test
file(GLOB TEST_SOURCES "*.cpp")
add_executable(run_tests ${TEST_SOURCES})

# Привязываем нашу библиотеку ЛР2 и Google Test
# Обрати внимание: lab1_lib здесь больше нет!
target_link_libraries(run_tests PRIVATE lab2_lib GTest::gtest_main)

# Убрали include(GoogleTest) и gtest_discover_tests, чтобы не было тайм-аута