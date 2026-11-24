cmake_minimum_required(VERSION 3.14)
project(OTIS_Task02 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# ВАЖНО: Добавляем флаги покрытия в самом начале, чтобы они действовали на все цели
add_compile_options(--coverage -O0)
add_link_options(--coverage)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(lab02_app src/main.cpp)
target_include_directories(lab02_app PRIVATE src)

add_executable(unit_tests test/test_main.cpp)
target_include_directories(unit_tests PRIVATE src)
target_link_libraries(unit_tests gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)