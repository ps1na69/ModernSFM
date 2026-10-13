#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>
#include <glm/glm.hpp>

namespace modern_sfm::core {

// Generational handle pattern for safe entity and resource references
template <typename Tag>
struct GenerationalHandle {
    uint32_t index{0};
    uint32_t generation{0};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return index != 0 || generation != 0;
    }

    constexpr auto operator<=>(const GenerationalHandle&) const noexcept = default;
};

// Generic error representation
struct Error {
    std::string message;
    int code{0};
};

template <typename T>
using Result = std::expected<T, Error>;

// Common math type aliases
using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
using Mat4 = glm::mat4;
using Color = glm::vec4;

} // namespace modern_sfm::core
