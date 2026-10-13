#include <modern_sfm/platform/window.hpp>

#include <stdexcept>

namespace modern_sfm::platform {

Window::Window(const WindowConfig& config) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        throw std::runtime_error(
            std::string("SDL_Init failed: ") + SDL_GetError());
    }

    SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;
    if (config.high_dpi) {
        flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;
    }

    m_window = SDL_CreateWindow(
        config.title.c_str(),
        config.width,
        config.height,
        flags);

    if (!m_window) {
        throw std::runtime_error(
            std::string("SDL_CreateWindow failed: ") + SDL_GetError());
    }

    refresh_size();
    MODERN_LOG_INFO("Window created: {}x{} (drawable: {}x{}), DPI scale: {:.2f}",
        static_cast<int>(m_size.x), static_cast<int>(m_size.y),
        static_cast<int>(m_drawable_size.x), static_cast<int>(m_drawable_size.y),
        m_dpi_scale);
}

Window::~Window() {
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    SDL_Quit();
}

void Window::poll_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Let external callback handle it first (e.g. ImGui, editor)
        if (m_event_cb && m_event_cb(event)) {
            continue;
        }

        switch (event.type) {
        case SDL_EVENT_QUIT:
            m_running = false;
            break;

        case SDL_EVENT_WINDOW_RESIZED:
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            refresh_size();
            break;

        default:
            break;
        }
    }
}

void Window::refresh_size() noexcept {
    int w{}, h{};
    SDL_GetWindowSize(m_window, &w, &h);
    m_size = {static_cast<float>(w), static_cast<float>(h)};

    int dw{}, dh{};
    SDL_GetWindowSizeInPixels(m_window, &dw, &dh);
    m_drawable_size = {static_cast<float>(dw), static_cast<float>(dh)};

    m_dpi_scale = (m_size.x > 0.0f)
        ? (m_drawable_size.x / m_size.x)
        : 1.0f;
}

} // namespace modern_sfm::platform
