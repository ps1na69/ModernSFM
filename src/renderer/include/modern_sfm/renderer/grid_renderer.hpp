#pragma once

#include <modern_sfm/renderer/gpu_context.hpp>
#include <modern_sfm/core/types.hpp>

namespace modern_sfm::renderer {

/// Draws a 3D world-space grid into an existing SDL_GPU render pass.
/// M1 vertical slice: CPU-generated line geometry uploaded each frame.
class GridRenderer {
public:
    explicit GridRenderer(GpuContext& ctx);
    ~GridRenderer();

    GridRenderer(const GridRenderer&) = delete;
    GridRenderer& operator=(const GridRenderer&) = delete;

    struct CameraUniforms {
        core::Mat4 view_proj;   // Combined view-projection matrix
        core::Vec4 grid_color;  // RGBA
    };

    /// Record grid draw commands into the active render pass.
    void render(SDL_GPUCommandBuffer* cmd,
                SDL_GPURenderPass*   pass,
                const CameraUniforms& cam);

private:
    GpuContext&     m_ctx;
    SDL_GPUBuffer*          m_vertex_buf{nullptr};
    SDL_GPUTransferBuffer*  m_transfer_buf{nullptr};
    SDL_GPUGraphicsPipeline* m_pipeline{nullptr};
    uint32_t        m_vertex_count{0};

    void build_pipeline(SDL_GPUTextureFormat swapchain_fmt);
    void upload_grid_vertices(int half_extent, float cell_size);
};

} // namespace modern_sfm::renderer
