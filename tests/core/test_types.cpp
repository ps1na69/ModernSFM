#include <doctest/doctest.h>
#include <modern_sfm/core/types.hpp>

using namespace modern_sfm::core;

TEST_CASE("GenerationalHandle default is invalid") {
    GenerationalHandle<struct TestTag> h{};
    CHECK_FALSE(h.is_valid());
}

TEST_CASE("GenerationalHandle with non-zero values is valid") {
    GenerationalHandle<struct TestTag> h{1, 0};
    CHECK(h.is_valid());
}

TEST_CASE("Result wraps success value") {
    const Result<int> r = 42;
    REQUIRE(r.has_value());
    CHECK(r.value() == 42);
}

TEST_CASE("Result wraps error") {
    const Result<int> r = std::unexpected(Error{"something failed", -1});
    REQUIRE_FALSE(r.has_value());
    CHECK(r.error().code == -1);
}
