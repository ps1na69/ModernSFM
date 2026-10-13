#pragma once

#include <chrono>
#include <deque>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

#include <spdlog/common.h>
#include <spdlog/details/log_msg.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/sinks/base_sink.h>

namespace modern_sfm::core {

struct LogEntry {
    std::chrono::system_clock::time_point timestamp;
    spdlog::level::level_enum level{spdlog::level::info};
    std::string logger_name;
    std::string message;
};

// Thread-safe spdlog sink capturing logs in a bounded ring buffer for the editor console
class EditorLogSink final : public spdlog::sinks::base_sink<std::mutex> {
public:
    explicit EditorLogSink(size_t max_entries = 1000)
        : max_entries_(max_entries) {
    }

    void clear() {
        std::lock_guard<std::mutex> lock(this->mutex_);
        entries_.clear();
    }

    [[nodiscard]] std::vector<LogEntry> get_entries() const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(this->mutex_));
        return std::vector<LogEntry>(entries_.begin(), entries_.end());
    }

    void for_each_entry(const std::function<void(const LogEntry&)>& visitor) const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(this->mutex_));
        for (const auto& entry : entries_) {
            visitor(entry);
        }
    }

    [[nodiscard]] size_t count() const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(this->mutex_));
        return entries_.size();
    }

    void set_max_entries(size_t max_entries) {
        std::lock_guard<std::mutex> lock(this->mutex_);
        max_entries_ = max_entries;
        while (entries_.size() > max_entries_) {
            entries_.pop_front();
        }
    }

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override {
        spdlog::memory_buf_t formatted;
        spdlog::sinks::base_sink<std::mutex>::formatter_->format(msg, formatted);

        LogEntry entry{
            .timestamp = msg.time,
            .level = msg.level,
            .logger_name = std::string(msg.logger_name.data(), msg.logger_name.size()),
            .message = fmt::to_string(formatted),
        };

        if (entries_.size() >= max_entries_) {
            entries_.pop_front();
        }
        entries_.push_back(std::move(entry));
    }

    void flush_() override {
    }

private:
    size_t max_entries_;
    std::deque<LogEntry> entries_;
};

} // namespace modern_sfm::core
