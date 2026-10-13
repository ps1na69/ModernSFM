#pragma once

#include <cstdint>
#include <string>
#include <functional>

#include <SDL3/SDL.h>
#include <modern_sfm/core/types.hpp>
#include <modern_sfm/core/log.hpp>

namespace modern_sfm::platform {

struct WindowConfig {
    std::string title{"ModernSFM"};
    int width{1600};
    int height{900};
    bool resizable{true};
    bool high_dpi{true};
};

/// RAII wrapper around SDL3 window + event loop.
/// Owns the SDL lifetime for the application.
class Window {
public:
    explicit Window(const WindowConfig& config);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    [[nodiscard]] bool is_running() const noexcept { return m_running; }
    [[nodiscard]] SDL_Window* handle() const noexcept { return m_window; }

    /// Logical size in OS points.
    [[nodiscard]] core::Vec2 size() const noexcept { return m_size; }
    /// Framebuffer pixel size (differs on high-DPI displays).
    [[nodiscard]] core::Vec2 drawable_size() const noexcept { return m_drawable_size; }
    [[nodiscard]] float dpi_scale() const noexcept { return m_dpi_scale; }

    using EventCallback = std::function<bool(const SDL_Event&)>;
    void set_event_callback(EventCallback cb) { m_event_cb = std::move(cb); }

    /// Poll events and dispatch them. Must be called each frame on the main thread.
    void poll_events();

    void request_quit() noexcept { m_running = false; }

private:
    SDL_Window*   m_window{nullptr};
    EventCallback m_event_cb;
    core::Vec2    m_size{};
    core::Vec2    m_drawable_size{};
    float         m_dpi_scale{1.0f};
    bool          m_running{true};

    void refresh_size() noexcept;
};

} // namespace modern_sfm::platform
