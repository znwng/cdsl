cmake_minimum_required(VERSION 3.20)

project(CDSL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------
# Dependencies
# ---------------------------------------------------------

add_subdirectory(external/replxx)

# ---------------------------------------------------------
# CDSL library
# ---------------------------------------------------------

file(GLOB_RECURSE CDSL_SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
)

# main.cpp belongs to the executable, not the library
list(REMOVE_ITEM CDSL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

add_library(cdsl_lib
    ${CDSL_SOURCES}
)

target_include_directories(cdsl_lib
    PUBLIC
    include
)

target_link_libraries(cdsl_lib
    PUBLIC
    replxx
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
