cmake_minimum_required(VERSION 3.20)

project(CDSL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------
# Dependencies
# ---------------------------------------------------------

add_subdirectory(external/CLI11)
add_subdirectory(external/replxx)

# ---------------------------------------------------------
# CDSL library
# ---------------------------------------------------------

add_library(cdsl_lib
    src/variables.cpp
    src/diagnostics.cpp
    src/expression.cpp
    src/file.cpp
    src/runtime.cpp
    src/validation.cpp

    src/instructions/set.cpp
    src/instructions/print.cpp
    src/instructions/move.cpp
    src/instructions/wait.cpp
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
