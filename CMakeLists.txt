cmake_minimum_required(VERSION 3.20)

project(CDSL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# ---------------------------------------------------------
# Dependencies
# ---------------------------------------------------------

add_subdirectory(external/CLI11)
add_subdirectory(external/replxx)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)

FetchContent_MakeAvailable(googletest)

# ---------------------------------------------------------
# CDSL library
# ---------------------------------------------------------

add_library(cdsl_lib
    src/constants.cpp
    src/diagnostics.cpp
    src/file.cpp
    src/runtime.cpp
    src/validation.cpp
)

target_include_directories(cdsl_lib
    PUBLIC
    include
)

target_link_libraries(cdsl_lib
    PUBLIC
    replxx
    CLI11::CLI11
)

# ---------------------------------------------------------
# Main executable
# ---------------------------------------------------------

add_executable(cdsl
    src/main.cpp
)

target_link_libraries(cdsl
    PRIVATE
    cdsl_lib
)

# ---------------------------------------------------------
# Tests
# ---------------------------------------------------------

enable_testing()

add_executable(cdsl_tests
    tests/test_runtime_process_instruction.cpp
    tests/test_validation.cpp
)

target_link_libraries(cdsl_tests
    PRIVATE
    cdsl_lib
    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(cdsl_tests)
