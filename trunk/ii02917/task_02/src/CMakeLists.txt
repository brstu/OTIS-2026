cmake_minimum_required(VERSION 3.14)
project(OTIS_Task02_ii02917 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(--coverage -O0)
  add_link_options(--coverage)
endif()

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(TASK02_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(TASK01_SRC "${TASK02_ROOT}/../task_01/src")

add_executable(ii02917_task02_app
  "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)
target_include_directories(ii02917_task02_app PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${TASK01_SRC}"
)

add_executable(ii02917_task02_tests
  "${TASK02_ROOT}/test/test_main.cpp"
)
target_include_directories(ii02917_task02_tests PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${TASK01_SRC}"
)
target_link_libraries(ii02917_task02_tests PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(ii02917_task02_tests)