add_library(modern_sfm_platform STATIC
    src/window.cpp
)

target_include_directories(modern_sfm_platform PUBLIC
    include
)

target_link_libraries(modern_sfm_platform PUBLIC
    modern_sfm_core
    SDL3::SDL3
)

target_compile_features(modern_sfm_platform PUBLIC
    cxx_std_23
)
