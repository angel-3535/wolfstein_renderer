cmake_minimum_required(VERSION 4.0.1)
project(wolf3d)

find_package(raylib 3.0 REQUIRED) # Requires at least version 3.0

set(CMAKE_C_STANDARD 11) # Requires C11 standard
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)



list(APPEND src_files src/main.c src/player.c src/gfx/renderer.c src/world/world.c)
add_executable(${PROJECT_NAME} ${src_files})

target_link_libraries(${PROJECT_NAME} raylib)

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
