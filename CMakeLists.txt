cmake_minimum_required(VERSION 3.15)
project(BlockBlast)

# Raylib FetchContent (DO NOT CHANGE!)
include(FetchContent)
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG        5.5
)
FetchContent_MakeAvailable(raylib)

# SOURCE CODES
set(SOURCES
    src/main.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

# .h FILES
target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} raylib)

# OS Linker (DO NOT CHANGE!)
if(APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
elseif(NOT MSVC)
    target_link_libraries(${PROJECT_NAME} m pthread dl)
endif()