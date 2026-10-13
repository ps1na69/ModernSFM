#include <modern_sfm/renderer/gpu_context.hpp>

#include <stdexcept>
#include <string>

namespace modern_sfm::renderer {

GpuContext::GpuContext(SDL_Window* window) : m_window(window) {
    // Declare all shader formats we can provide (AOT compiled SPIR-V and DXIL)
    const SDL_GPUShaderFormat formats =
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL;

    // Create device with debug mode in debug builds
#if defined(NDEBUG)
    constexpr bool debug_mode = false;
#else
    constexpr bool debug_mode = true;
#endif

    m_device = SDL_CreateGPUDevice(formats, debug_mode, nullptr);
    if (!m_device) {
        throw std::runtime_error(
            std::string("SDL_CreateGPUDevice failed: ") + SDL_GetError());
    }

    if (!SDL_ClaimWindowForGPUDevice(m_device, m_window)) {
        SDL_DestroyGPUDevice(m_device);
        m_device = nullptr;
        throw std::runtime_error(
            std::string("SDL_ClaimWindowForGPUDevice failed: ") + SDL_GetError());
    }

    m_swapchain_format = SDL_GetGPUSwapchainTextureFormat(m_device, m_window);

    MODERN_LOG_INFO("GPU device created. Backend: {}, Swapchain format: {}",
        SDL_GetGPUDeviceDriver(m_device),
        static_cast<int>(m_swapchain_format));
}

GpuContext::~GpuContext() {
    if (m_device) {
        SDL_ReleaseWindowFromGPUDevice(m_device, m_window);
        SDL_DestroyGPUDevice(m_device);
        m_device = nullptr;
    }
}

} // namespace modern_sfm::renderer
