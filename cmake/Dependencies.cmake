# CMake dependency management via FetchContent
include(FetchContent)

# -----------------------------------------------------------------------------
# SDL3
# -----------------------------------------------------------------------------
set(SDL_SHARED ON CACHE BOOL "Build SDL3 as shared library" FORCE)
set(SDL_STATIC OFF CACHE BOOL "Build SDL3 as static library" FORCE)
set(SDL_TESTS OFF CACHE BOOL "Disable SDL3 tests" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "Disable SDL3 examples" FORCE)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.0
    GIT_SHALLOW TRUE
)

# -----------------------------------------------------------------------------
# spdlog
# -----------------------------------------------------------------------------
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "Disable spdlog tests" FORCE)
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "Disable spdlog examples" FORCE)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.14.1
    GIT_SHALLOW TRUE
)

# -----------------------------------------------------------------------------
# glm
# -----------------------------------------------------------------------------
set(GLM_BUILD_TESTS OFF CACHE BOOL "Disable GLM tests" FORCE)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE
)

# -----------------------------------------------------------------------------
# doctest
# -----------------------------------------------------------------------------
set(DOCTEST_WITH_TESTS OFF CACHE BOOL "Disable doctest internal tests" FORCE)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
    GIT_SHALLOW TRUE
    DOWNLOAD_ONLY TRUE
)

# -----------------------------------------------------------------------------
# Dear ImGui (docking branch)
# -----------------------------------------------------------------------------
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
    GIT_SHALLOW TRUE
    DOWNLOAD_ONLY TRUE
)

# Make regular dependencies available
FetchContent_MakeAvailable(SDL3 spdlog glm imgui)

# Populate doctest as header-only without invoking its CMakeLists.txt
FetchContent_GetProperties(doctest)
if(NOT doctest_POPULATED)
    FetchContent_Populate(doctest)
    add_library(doctest_interface INTERFACE)
    target_include_directories(doctest_interface INTERFACE ${doctest_SOURCE_DIR})
    add_library(doctest::doctest ALIAS doctest_interface)
endif()

# Define ImGui static library with SDL3 + SDL_GPU backends
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_compile_definitions(imgui PUBLIC
    IMGUI_DEFINE_MATH_OPERATORS
)

target_link_libraries(imgui PUBLIC
    SDL3::SDL3
)

target_compile_features(imgui PUBLIC
    cxx_std_23
)
