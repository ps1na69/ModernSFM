#pragma once

#include <memory>
#include <string>

#include <spdlog/spdlog.h>
#include <modern_sfm/core/log_sink.hpp>

namespace modern_sfm::core {

class Logger {
public:
    static void init(const std::string& logger_name = "ModernSFM");
    static void shutdown();

    [[nodiscard]] static std::shared_ptr<spdlog::logger> get();
    [[nodiscard]] static std::shared_ptr<EditorLogSink> get_editor_sink();

private:
    static std::shared_ptr<spdlog::logger> s_logger;
    static std::shared_ptr<EditorLogSink> s_editor_sink;
};

} // namespace modern_sfm::core

#define MODERN_LOG_TRACE(...)    ::modern_sfm::core::Logger::get()->trace(__VA_ARGS__)
#define MODERN_LOG_DEBUG(...)    ::modern_sfm::core::Logger::get()->debug(__VA_ARGS__)
#define MODERN_LOG_INFO(...)     ::modern_sfm::core::Logger::get()->info(__VA_ARGS__)
#define MODERN_LOG_WARN(...)     ::modern_sfm::core::Logger::get()->warn(__VA_ARGS__)
#define MODERN_LOG_ERROR(...)    ::modern_sfm::core::Logger::get()->error(__VA_ARGS__)
#define MODERN_LOG_CRITICAL(...) ::modern_sfm::core::Logger::get()->critical(__VA_ARGS__)
