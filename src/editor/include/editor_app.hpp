#pragma once

#include <modern_sfm/platform/window.hpp>
#include <modern_sfm/renderer/gpu_context.hpp>
#include <modern_sfm/renderer/grid_renderer.hpp>

namespace modern_sfm::editor {

/// Top-level application object.
/// Owns all subsystems, drives the main loop.
class EditorApp {
public:
    EditorApp();
    ~EditorApp();

    EditorApp(const EditorApp&) = delete;
    EditorApp& operator=(const EditorApp&) = delete;

    /// Run until the user closes the window. Returns 0 on success.
    int run();

private:
    std::unique_ptr<platform::Window>          m_window;
    std::unique_ptr<renderer::GpuContext>      m_gpu;
    std::unique_ptr<renderer::GridRenderer>    m_grid;

    bool m_imgui_initialized{false};

    void init_imgui();
    void shutdown_imgui();

    bool on_event(const SDL_Event& event);
    void render_frame();
    void build_ui();
};

} // namespace modern_sfm::editor
