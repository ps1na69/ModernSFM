#include <editor_app.hpp>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>

#include <modern_sfm/core/log.hpp>

namespace modern_sfm::editor {

EditorApp::EditorApp() {
    platform::WindowConfig win_cfg{
        .title   = "ModernSFM — M1 Prototype",
        .width   = 1600,
        .height  = 900,
        .resizable = true,
        .high_dpi  = true,
    };

    m_window = std::make_unique<platform::Window>(win_cfg);
    m_gpu    = std::make_unique<renderer::GpuContext>(m_window->handle());
    m_grid   = std::make_unique<renderer::GridRenderer>(*m_gpu);

    init_imgui();

    m_window->set_event_callback([this](const SDL_Event& e) {
        return on_event(e);
    });

    MODERN_LOG_INFO("EditorApp initialised.");
}

EditorApp::~EditorApp() {
    shutdown_imgui();
}

void EditorApp::init_imgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // DPI-aware font scaling
    const float dpi = m_window->dpi_scale();
    io.FontGlobalScale = dpi;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(dpi);

    ImGui_ImplSDL3_InitForSDLGPU(m_window->handle());

    ImGui_ImplSDLGPU3_InitInfo gpu_info{};
    gpu_info.Device              = m_gpu->device();
    gpu_info.ColorTargetFormat   = m_gpu->swapchain_format();
    gpu_info.MSAASamples         = SDL_GPU_SAMPLECOUNT_1;
    ImGui_ImplSDLGPU3_Init(&gpu_info);

    m_imgui_initialized = true;
    MODERN_LOG_INFO("Dear ImGui {}, docking enabled, DPI scale: {:.2f}",
        ImGui::GetVersion(), dpi);
}

void EditorApp::shutdown_imgui() {
    if (m_imgui_initialized) {
        SDL_WaitForGPUIdle(m_gpu->device());
        ImGui_ImplSDLGPU3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        m_imgui_initialized = false;
    }
}

bool EditorApp::on_event(const SDL_Event& event) {
    ImGui_ImplSDL3_ProcessEvent(&event);

    if (event.type == SDL_EVENT_QUIT) {
        m_window->request_quit();
        return true;
    }
    if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
        m_window->request_quit();
        return true;
    }
    return false;
}

int EditorApp::run() {
    MODERN_LOG_INFO("Entering main loop.");

    while (m_window->is_running()) {
        m_window->poll_events();
        render_frame();
    }

    // Wait for all GPU work to finish before destroying resources.
    SDL_WaitForGPUIdle(m_gpu->device());
    MODERN_LOG_INFO("Main loop exited cleanly.");
    return 0;
}

void EditorApp::render_frame() {
    // --- ImGui frame start ---
    ImGui_ImplSDLGPU3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    build_ui();

    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();

    // --- Acquire GPU command buffer ---
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(m_gpu->device());
    if (!cmd) {
        MODERN_LOG_ERROR("SDL_AcquireGPUCommandBuffer failed: {}", SDL_GetError());
        return;
    }

    // --- Acquire swapchain texture ---
    SDL_GPUTexture* swapchain_tex = nullptr;
    Uint32 sw{}, sh{};
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd, m_window->handle(),
                                               &swapchain_tex, &sw, &sh)) {
        MODERN_LOG_ERROR("SDL_WaitAndAcquireGPUSwapchainTexture failed: {}", SDL_GetError());
        SDL_CancelGPUCommandBuffer(cmd);
        return;
    }

    if (swapchain_tex) {
        // Step A (CRITICAL): prepare ImGui draw data BEFORE opening render pass
        ImGui_ImplSDLGPU3_PrepareDrawData(draw_data, cmd);

        SDL_GPUColorTargetInfo color_target{};
        color_target.texture       = swapchain_tex;
        color_target.clear_color   = {0.118f, 0.118f, 0.118f, 1.0f};
        color_target.load_op       = SDL_GPU_LOADOP_CLEAR;
        color_target.store_op      = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(cmd, &color_target, 1, nullptr);

        // Grid render (no-op until shaders compiled in M1.5)
        renderer::GridRenderer::CameraUniforms cam{
            .view_proj  = glm::mat4(1.0f),
            .grid_color = {0.4f, 0.4f, 0.4f, 1.0f},
        };
        m_grid->render(cmd, pass, cam);

        // Step B: draw ImGui INSIDE render pass
        ImGui_ImplSDLGPU3_RenderDrawData(draw_data, cmd, pass);

        SDL_EndGPURenderPass(pass);
    }

    SDL_SubmitGPUCommandBuffer(cmd);
}

void EditorApp::build_ui() {
    // Full-screen DockSpace
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags host_flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize   | ImGuiWindowFlags_NoMove     |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_MenuBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
    ImGui::Begin("##DockSpaceHost", nullptr, host_flags);
    ImGui::PopStyleVar(3);

    // --- Menu Bar ---
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Session",  "Ctrl+N")) {}
            if (ImGui::MenuItem("Open Session", "Ctrl+O")) {}
            if (ImGui::MenuItem("Save Session", "Ctrl+S")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                m_window->request_quit();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Viewport"))          {}
            if (ImGui::MenuItem("Animation Set Editor")) {}
            if (ImGui::MenuItem("Clip Editor"))       {}
            if (ImGui::MenuItem("Graph Editor"))      {}
            if (ImGui::MenuItem("Element Viewer"))    {}
            if (ImGui::MenuItem("Log Console"))       {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {
            if (ImGui::MenuItem("Reset Layout")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About ModernSFM")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // DockSpace
    const ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2{0.0f, 0.0f},
                     ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();

    // -----------------------------------------------------------------------
    // Panels (all shown by default; can be docked/rearranged)
    // -----------------------------------------------------------------------

    // Viewport panel
    if (ImGui::Begin("Viewport")) {
        const ImVec2 region = ImGui::GetContentRegionAvail();
        ImGui::TextDisabled("3D Viewport (%.0f x %.0f)", region.x, region.y);
        ImGui::TextDisabled("Grid renderer active | Shaders: M1.5");
    }
    ImGui::End();

    // Animation Set Editor placeholder
    if (ImGui::Begin("Animation Set Editor")) {
        ImGui::TextDisabled("Animation sets will appear here (M4)");
        ImGui::Separator();
        ImGui::TextDisabled("No session loaded.");
    }
    ImGui::End();

    // Clip Editor / Timeline placeholder
    if (ImGui::Begin("Clip Editor")) {
        ImGui::TextDisabled("Timeline tracks will appear here (M5/M6)");
    }
    ImGui::End();

    // Graph Editor placeholder
    if (ImGui::Begin("Graph Editor")) {
        ImGui::TextDisabled("Animation curve graph editor (M6)");
    }
    ImGui::End();

    // Element Viewer / Inspector placeholder
    if (ImGui::Begin("Element Viewer")) {
        ImGui::TextDisabled("DMX element tree will appear here (M5)");
    }
    ImGui::End();

    // Log Console
    if (ImGui::Begin("Log Console")) {
        if (ImGui::SmallButton("Clear")) {
            core::Logger::get_editor_sink()->clear();
        }
        ImGui::SameLine();
        ImGui::TextDisabled("%zu entries",
            core::Logger::get_editor_sink()->count());
        ImGui::Separator();

        ImGui::BeginChild("##log_scroll", ImVec2{0.0f, 0.0f}, false,
                          ImGuiWindowFlags_HorizontalScrollbar);

        core::Logger::get_editor_sink()->for_each_entry(
            [](const core::LogEntry& e) {
                ImVec4 col{1.0f, 1.0f, 1.0f, 1.0f};
                switch (e.level) {
                case spdlog::level::warn:     col = {1.0f, 0.85f, 0.0f, 1.0f}; break;
                case spdlog::level::err:      col = {1.0f, 0.3f, 0.3f, 1.0f};  break;
                case spdlog::level::critical: col = {1.0f, 0.0f, 0.0f, 1.0f};  break;
                case spdlog::level::debug:    col = {0.7f, 0.7f, 0.7f, 1.0f};  break;
                case spdlog::level::trace:    col = {0.5f, 0.5f, 0.5f, 1.0f};  break;
                default: break;
                }
                ImGui::TextColored(col, "%s", e.message.c_str());
            });

        // Auto-scroll to bottom
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

} // namespace modern_sfm::editor
