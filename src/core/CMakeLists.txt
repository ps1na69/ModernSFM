add_library(modern_sfm_core STATIC
    src/log.cpp
)

target_include_directories(modern_sfm_core PUBLIC
    include
)

target_link_libraries(modern_sfm_core PUBLIC
    spdlog::spdlog
    glm::glm
)

target_compile_features(modern_sfm_core PUBLIC
    cxx_std_23
)
