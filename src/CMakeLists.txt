set(CXX_FLAGS
    "-Wall"
    "-Wextra"
    "-Werror"
    "-pthread"
    "-std=c++1y"
    "-pedantic"
    "-fno-rtti"
    "-fPIE")

foreach(FLAG ${CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}")
endforeach()

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(LTO_FLAGS "-flto -fno-use-linker-plugin -fwhole-program")
endif()

set(OPT_FLAGS "-ffast-math -funroll-all-loops -march=native -Ofast")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -march=native ${LTO_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG ${OPT_FLAGS} ${LTO_FLAGS}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g ${LTO_FLAGS}")

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined -O2 -fPIE")
set(CMAKE_EXE_LINKER_FLAGS "${LTO_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${LTO_FLAGS}")

set(EXECUTALBE_NAME ${PROJECT_NAME})

configure_file("${PROJECT_SOURCE_DIR}/config.h.in"
               "${PROJECT_BINARY_DIR}/conf.h")
include_directories("${PROJECT_BINARY_DIR}")

add_library(utils utils.cpp)
add_library(libtr transform.cpp)
add_executable(${EXECUTALBE_NAME} main.cpp)

set(CMAKE_MODULE_PATH "/usr/share/SFML/cmake/Modules/;${CMAKE_MODULE_PATH}")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules;${CMAKE_MODULE_PATH}")
find_package(SFML 2 REQUIRED system window graphics)
include_directories(${SFML_INCLUDE_DIR})

find_package(Work REQUIRED)
include_directories(${Work_INCLUDE_DIR})

target_link_libraries(libtr ${SFML_LIBRARIES})
target_link_libraries(libtr utils)
target_link_libraries(${EXECUTALBE_NAME} libtr)

install(TARGETS ${EXECUTALBE_NAME} DESTINATION bin)
