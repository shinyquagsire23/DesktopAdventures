cmake_minimum_required(VERSION 3.4)
project(DesktopAdventures)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
file(MAKE_DIRECTORY ${DesktopAdventures_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DesktopAdventures_SOURCE_DIR}/bin)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/cmake-modules")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
find_package(SDL2_mixer REQUIRED)
include_directories(${SDLMIXER_INCLUDE_DIR})
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

include_directories(${DesktopAdventures_SOURCE_DIR}/src/include)
include_directories(${DesktopAdventures_SOURCE_DIR}/src/pc/)

set(SOURCE_FILES
    src/pc/main.c
    src/pc/main.h
    src/assets.c
    src/include/assets.h
    src/character.c
    src/include/character.h
    src/include/input.h
    src/map.c
    src/include/map.h
    src/include/objectinfo.h
    src/include/palette.h
    src/player.c
    src/include/player.h
    src/puzzle.c
    src/include/puzzle.h
    src/screen.c
    src/include/screen.h
    src/include/sound.h
    src/include/tile.h
    src/include/tname.h
    src/include/useful.h
    src/include/input.h
    src/input.c src/iact.c
    src/include/iact.h
    src/pc/sound.c src/render_gl.c src/render_buffer.c src/font.c src/include/font.h src/palette.c)

add_definitions(-DLITTLE_ENDIAN)
add_definitions(-DPC_BUILD)
add_definitions(-DRENDER_BUFFER)
add_definitions(-DDAT_IN_RAM)
set( CMAKE_VERBOSE_MAKEFILE on )

add_executable(DesktopAdventures ${SOURCE_FILES})
target_link_libraries(DesktopAdventures ${SDL2_LIBRARY} ${SDLMIXER_LIBRARY} ${OPENGL_LIBRARY})
