cmake_minimum_required(VERSION 2.8)

project(invert-sfml)

include(Compiler.cmake)

include_directories(${PROJECT_SOURCE_DIR}/src)
add_subdirectory(src)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "2")
set(VERSION_PATCH "1")

include(DebPack.cmake)
