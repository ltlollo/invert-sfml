cmake_minimum_required(VERSION 2.8)

project(transform-img)

include(Compiler.cmake)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "4")
set(VERSION_PATCH "0")

add_subdirectory(src)

include(DebPack.cmake)
