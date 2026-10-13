#include <editor_app.hpp>
#include <modern_sfm/core/log.hpp>

#include <exception>
#include <cstdlib>

int main(int /*argc*/, char* /*argv*/[]) {
    modern_sfm::core::Logger::init("ModernSFM");
    MODERN_LOG_INFO("ModernSFM M1 starting up.");

    try {
        modern_sfm::editor::EditorApp app;
        const int result = app.run();
        modern_sfm::core::Logger::shutdown();
        return result;
    } catch (const std::exception& ex) {
        MODERN_LOG_CRITICAL("Fatal exception: {}", ex.what());
        modern_sfm::core::Logger::shutdown();
        return EXIT_FAILURE;
    }
}
