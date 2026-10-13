add_library(modern_sfm_renderer STATIC
    src/gpu_context.cpp
    src/grid_renderer.cpp
)

target_include_directories(modern_sfm_renderer PUBLIC
    include
)

target_link_libraries(modern_sfm_renderer PUBLIC
    modern_sfm_core
    modern_sfm_platform
    imgui
    SDL3::SDL3
)

target_compile_features(modern_sfm_renderer PUBLIC
    cxx_std_23
)
