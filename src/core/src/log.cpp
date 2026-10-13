#include <modern_sfm/core/log.hpp>

#include <vector>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace modern_sfm::core {

std::shared_ptr<spdlog::logger> Logger::s_logger = nullptr;
std::shared_ptr<EditorLogSink> Logger::s_editor_sink = nullptr;

void Logger::init(const std::string& logger_name) {
    if (s_logger != nullptr) {
        return;
    }

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] %v");

    s_editor_sink = std::make_shared<EditorLogSink>(2000);
    s_editor_sink->set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");

    std::vector<spdlog::sink_ptr> sinks{console_sink, s_editor_sink};
    s_logger = std::make_shared<spdlog::logger>(logger_name, sinks.begin(), sinks.end());
    s_logger->set_level(spdlog::level::trace);
    s_logger->flush_on(spdlog::level::trace);

    spdlog::register_logger(s_logger);
    spdlog::set_default_logger(s_logger);
}

void Logger::shutdown() {
    spdlog::drop_all();
    s_logger.reset();
    s_editor_sink.reset();
}

std::shared_ptr<spdlog::logger> Logger::get() {
    if (s_logger == nullptr) {
        init();
    }
    return s_logger;
}

std::shared_ptr<EditorLogSink> Logger::get_editor_sink() {
    if (s_editor_sink == nullptr) {
        init();
    }
    return s_editor_sink;
}

} // namespace modern_sfm::core
