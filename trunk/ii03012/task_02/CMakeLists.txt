cmake_minimum_required(VERSION 3.21)
project(OTIS_ii03012_task_02 LANGUAGES CXX)
add_subdirectory(src)
include(CTest)
if(BUILD_TESTING)
    include(FetchContent)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG 52eb8108c5bdec04579160ae17225d66034bd723)
    FetchContent_MakeAvailable(googletest)
    add_executable(ii03012_tests test/models_test.cpp)
    target_link_libraries(ii03012_tests PRIVATE ii03012_models GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(ii03012_tests NO_PRETTY_VALUES)
    add_test(NAME ConsoleScenarios COMMAND ${CMAKE_COMMAND}
        -DPROGRAM=$<TARGET_FILE:ii03012_task_02> -P ${CMAKE_CURRENT_SOURCE_DIR}/test/console.cmake)
endif()
