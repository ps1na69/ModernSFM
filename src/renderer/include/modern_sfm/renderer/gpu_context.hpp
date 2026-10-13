#pragma once

#include <SDL3/SDL.h>
#include <modern_sfm/core/log.hpp>

namespace modern_sfm::renderer {

/// Owns the SDL_GPUDevice and swapchain claim for a single window.
class GpuContext {
public:
    explicit GpuContext(SDL_Window* window);
    ~GpuContext();

    GpuContext(const GpuContext&) = delete;
    GpuContext& operator=(const GpuContext&) = delete;
    GpuContext(GpuContext&&) = delete;
    GpuContext& operator=(GpuContext&&) = delete;

    [[nodiscard]] SDL_GPUDevice* device() const noexcept { return m_device; }
    [[nodiscard]] SDL_Window*    window() const noexcept { return m_window; }
    [[nodiscard]] SDL_GPUTextureFormat swapchain_format() const noexcept {
        return m_swapchain_format;
    }

private:
    SDL_Window*         m_window{nullptr};
    SDL_GPUDevice*      m_device{nullptr};
    SDL_GPUTextureFormat m_swapchain_format{SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM};
};

} // namespace modern_sfm::renderer
