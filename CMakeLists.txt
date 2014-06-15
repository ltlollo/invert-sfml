cmake_minimum_required(VERSION 2.8)

project(invert)

include(Compiler.cmake)

include_directories(${PROJECT_SOURCE_DIR}/src)
add_subdirectory(src)

set(invert_VERSION_MAJOR "0")
set(invert_VERSION_MINOR "1")

include(DebPack.cmake)
