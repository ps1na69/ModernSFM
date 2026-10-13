#include <modern_sfm/renderer/grid_renderer.hpp>

#include <vector>
#include <stdexcept>
#include <cstring>
#include <array>

#include <glm/gtc/matrix_transform.hpp>

namespace modern_sfm::renderer {

// --------------------------------------------------------------------------
// Embedded SPIR-V shaders for the grid lines (vertex color passthrough)
// Generated from simple GLSL; embedded as byte arrays to avoid runtime deps.
// For M1, we use SDL_GPU_SHADERFORMAT_SPIRV on Vulkan, DXIL on D3D12.
// --------------------------------------------------------------------------

// Simple coloured-line vertex shader (GLSL source reference):
//   layout(push_constant) uniform Uniforms { mat4 view_proj; vec4 color; };
//   layout(location=0) in vec3 inPos;
//   void main() { gl_Position = view_proj * vec4(inPos, 1.0); }
//
// Fragment shader:
//   layout(push_constant) uniform Uniforms { mat4 view_proj; vec4 color; };
//   layout(location=0) out vec4 outColor;
//   void main() { outColor = color; }
//
// For M1 prototype: We create the pipeline but draw nothing if shaders aren't
// precompiled yet. The grid upload still works; we'll integrate real shaders
// in the first CI step. This keeps the build green and architecture correct.

namespace {

struct GridVertex {
    float x, y, z;
};

std::vector<GridVertex> build_grid_lines(int half_extent, float cell_size) {
    std::vector<GridVertex> verts;
    const float limit = static_cast<float>(half_extent) * cell_size;

    for (int i = -half_extent; i <= half_extent; ++i) {
        const float pos = static_cast<float>(i) * cell_size;
        // Line along X axis
        verts.push_back({-limit, 0.0f, pos});
        verts.push_back({ limit, 0.0f, pos});
        // Line along Z axis
        verts.push_back({pos, 0.0f, -limit});
        verts.push_back({pos, 0.0f,  limit});
    }
    return verts;
}

} // namespace

GridRenderer::GridRenderer(GpuContext& ctx) : m_ctx(ctx) {
    upload_grid_vertices(20, 1.0f);
    // Pipeline construction deferred until shader bytecode is available (M1.5)
    MODERN_LOG_INFO("GridRenderer initialised ({} line vertices)", m_vertex_count);
}

GridRenderer::~GridRenderer() {
    if (m_vertex_buf) {
        SDL_ReleaseGPUBuffer(m_ctx.device(), m_vertex_buf);
    }
    if (m_transfer_buf) {
        SDL_ReleaseGPUTransferBuffer(m_ctx.device(), m_transfer_buf);
    }
    if (m_pipeline) {
        SDL_ReleaseGPUGraphicsPipeline(m_ctx.device(), m_pipeline);
    }
}

void GridRenderer::upload_grid_vertices(int half_extent, float cell_size) {
    const auto verts = build_grid_lines(half_extent, cell_size);
    m_vertex_count = static_cast<uint32_t>(verts.size());
    const uint32_t byte_size = m_vertex_count * sizeof(GridVertex);

    // Create GPU vertex buffer
    SDL_GPUBufferCreateInfo buf_info{};
    buf_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    buf_info.size  = byte_size;
    m_vertex_buf = SDL_CreateGPUBuffer(m_ctx.device(), &buf_info);
    if (!m_vertex_buf) {
        throw std::runtime_error(
            std::string("Failed to create grid vertex buffer: ") + SDL_GetError());
    }

    // Upload via transfer buffer (cycle=false: one-time upload)
    SDL_GPUTransferBufferCreateInfo tbuf_info{};
    tbuf_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    tbuf_info.size  = byte_size;
    m_transfer_buf = SDL_CreateGPUTransferBuffer(m_ctx.device(), &tbuf_info);
    if (!m_transfer_buf) {
        throw std::runtime_error(
            std::string("Failed to create transfer buffer: ") + SDL_GetError());
    }

    void* mapped = SDL_MapGPUTransferBuffer(m_ctx.device(), m_transfer_buf, false);
    std::memcpy(mapped, verts.data(), byte_size);
    SDL_UnmapGPUTransferBuffer(m_ctx.device(), m_transfer_buf);

    // Record upload on a one-shot command buffer
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(m_ctx.device());
    SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(cmd);

    SDL_GPUTransferBufferLocation src{};
    src.transfer_buffer = m_transfer_buf;
    src.offset = 0;

    SDL_GPUBufferRegion dst{};
    dst.buffer = m_vertex_buf;
    dst.offset = 0;
    dst.size   = byte_size;

    SDL_UploadToGPUBuffer(copy, &src, &dst, false);
    SDL_EndGPUCopyPass(copy);
    SDL_SubmitGPUCommandBuffer(cmd);
}

void GridRenderer::render(SDL_GPUCommandBuffer* /*cmd*/,
                          SDL_GPURenderPass*   /*pass*/,
                          const CameraUniforms& /*cam*/) {
    // TODO(M1.5): Bind m_pipeline, push CameraUniforms, draw m_vertex_count lines
    // Deferred until embedded SPIR-V/DXIL shaders are compiled in CI.
}

} // namespace modern_sfm::renderer
