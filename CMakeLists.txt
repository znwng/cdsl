cmake_minimum_required(VERSION 3.20)

project(CDSL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(external/CLI11)
add_subdirectory(external/replxx)

add_executable(cdsl
    src/main.cpp
    src/constants.cpp
    src/diagnostics.cpp
    src/file.cpp
    src/runtime.cpp
    src/validation.cpp
)

target_include_directories(cdsl PRIVATE include)
target_link_libraries(cdsl PRIVATE replxx)
target_link_libraries(cdsl PRIVATE CLI11::CLI11)

