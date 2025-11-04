cmake_minimum_required(VERSION 4.0.1)
project(wolf3d)

find_package(raylib 3.0 REQUIRED) # Requires at least version 3.0

set(CMAKE_C_STANDARD 11) # Requires C11 standard
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "Debug build")
  set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG")
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Release")
  message(STATUS "Release build")
  set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

list(APPEND   SRC_FILES 
  src/main.c 
  src/entity/player.c 
  src/gfx/renderer.c 
  src/world/world.c
  src/engine/scenes/main_map.c
  src/engine/scenes/main_menu.c

)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} raylib)

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()

list(
  APPEND EXTRA_INCLUDES 
  "${PROJECT_SOURCE_DIR}/src"
)

target_include_directories(${PROJECT_NAME} PRIVATE ${EXTRA_INCLUDES})

