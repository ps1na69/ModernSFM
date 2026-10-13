cmake_minimum_required(VERSION 3.28)

project(ModernSFM
    VERSION 0.1.0
    DESCRIPTION "Modern reimplementation of Source Filmmaker built with C++23, SDL3, SDL_GPU, and Dear ImGui"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compile commands for clang-tidy / IDE tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# MSVC specific compiler and warning flags
if(MSVC)
    add_compile_options(
        /W4
        /permissive-
        /utf-8
        /Zc:__cplusplus
        /EHsc
    )
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Dependencies
include(cmake/Dependencies.cmake)

# Subdirectories
add_subdirectory(src/core)
add_subdirectory(src/platform)
add_subdirectory(src/renderer)
add_subdirectory(src/editor)

enable_testing()
add_subdirectory(tests)
