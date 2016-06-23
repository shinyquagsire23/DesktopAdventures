cmake_minimum_required(VERSION 3.4)
project(DesktopAdventures)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
file(MAKE_DIRECTORY ${DesktopAdventures_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DesktopAdventures_SOURCE_DIR}/bin)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/cmake-modules")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

set(SOURCE_FILES
    pc/main.c
    pc/main.h
    assets.c
    assets.h
    character.c
    character.h
    input.h
    map.c
    map.h
    objectinfo.h
    palette.h
    puzzle.c
    puzzle.h
    screen.c
    screen.h
    sound.h
    tile.h
    tname.h
    useful.c
    useful.h)

add_definitions(-DPC_BUILD)
set( CMAKE_VERBOSE_MAKEFILE on )

add_executable(DesktopAdventures ${SOURCE_FILES})
target_link_libraries(DesktopAdventures ${SDL2_LIBRARY} ${OPENGL_LIBRARY})
