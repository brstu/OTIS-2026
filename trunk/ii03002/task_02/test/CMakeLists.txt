# GoogleTest: use the bundled copy (works offline). If it is missing,
# fall back to downloading it from GitHub.
set(GTEST_LOCAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/googletest")

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

if(EXISTS "${GTEST_LOCAL_DIR}/CMakeLists.txt")
    add_subdirectory("${GTEST_LOCAL_DIR}" "${CMAKE_BINARY_DIR}/googletest" EXCLUDE_FROM_ALL)
else()
    include(FetchContent)
    if(POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
    endif()
    FetchContent_Declare(googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    )
    FetchContent_MakeAvailable(googletest)
endif()

add_executable(ii03002_task02_tests
    test_models.cpp
    test_stability.cpp
    test_signals.cpp
)
target_link_libraries(ii03002_task02_tests PRIVATE ii03002_task01_models GTest::gtest_main)
target_include_directories(ii03002_task02_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../task_01/src"
)
if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(ii03002_task02_tests PRIVATE -O0 -g --coverage)
    target_link_options(ii03002_task02_tests PRIVATE --coverage)
endif()
include(GoogleTest)
gtest_discover_tests(ii03002_task02_tests)
