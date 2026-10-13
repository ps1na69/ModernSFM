add_executable(modern_sfm_editor
    src/main.cpp
    src/editor_app.cpp
    src/panels/viewport_panel.cpp
    src/panels/log_console_panel.cpp
)

target_include_directories(modern_sfm_editor PRIVATE
    include
)

target_link_libraries(modern_sfm_editor PRIVATE
    modern_sfm_core
    modern_sfm_platform
    modern_sfm_renderer
    imgui
    SDL3::SDL3
)

target_compile_features(modern_sfm_editor PRIVATE
    cxx_std_23
)

# On Windows, embed an application manifest and set subsystem to WINDOWS
# in release but keep CONSOLE for debug to see log output.
if(MSVC)
    target_compile_options(modern_sfm_editor PRIVATE /wd4100)
endif()

# Copy SDL3.dll next to the executable after build
add_custom_command(TARGET modern_sfm_editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE_DIR:modern_sfm_editor>
    COMMENT "Copying SDL3.dll to output directory"
)
