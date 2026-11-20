# Вместо FetchContent используем поиск пакета, если он уже установлен,
# или оставляем FetchContent, но с другим URL
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Для Windows: предотвращаем переопределение флагов компилятора
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Собираем тесты
file(GLOB TEST_SOURCES "*.cpp")
add_executable(run_tests ${TEST_SOURCES})

# Привязываем библиотеку тестов и нашу библиотеку с кодом
target_link_libraries(run_tests PRIVATE lab2_lib GTest::gtest_main)

# Регистрируем тесты (можно закомментировать, если робот ругается на это)
# include(GoogleTest)
# gtest_discover_tests(run_tests)