#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <modern_sfm/core/log.hpp>
#include <modern_sfm/core/log_sink.hpp>

TEST_CASE("Logger initialises without throwing") {
    CHECK_NOTHROW(modern_sfm::core::Logger::init("test_logger"));
}

TEST_CASE("Logger captures messages in EditorLogSink") {
    modern_sfm::core::Logger::init("test_capture");
    auto sink = modern_sfm::core::Logger::get_editor_sink();
    REQUIRE(sink != nullptr);

    sink->clear();
    const size_t before = sink->count();
    MODERN_LOG_INFO("Test message for sink capture");
    const size_t after = sink->count();

    CHECK(after == before + 1);
    modern_sfm::core::Logger::shutdown();
}
