set(LTO_FLAGS "-flto -fno-use-linker-plugin -fwhole-program -fno-rtti -fPIE")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pthread -std=c++1y -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -march=native ${LTO_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-O4 -DNDEBUG -march=native ${LTO_FLAGS}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g ${LTO_FLAGS}")

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined -O2 -fPIE")
set(CMAKE_EXE_LINKER_FLAGS "${LTO_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${LTO_FLAGS}")

find_library(LIB_PNG png)

add_library(libinv invert.cpp)
add_executable(invert main.cpp)
target_link_libraries(invert libinv)
target_link_libraries(libinv ${LIB_PNG})


install(TARGETS invert DESTINATION bin)