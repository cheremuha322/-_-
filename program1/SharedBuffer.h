#pragma once

#include <condition_variable>
#include <mutex>
#include <string>

class SharedBuffer {
public:
    void push(std::string value) {
        std::unique_lock<std::mutex> lock(mutex_);

        free_cv_.wait(lock, [this] {
            return !has_value_ || shutdown_;
        });

        if (shutdown_) return;

        value_ = static_cast<std::string&&>(value); // move без <utility>
        has_value_ = true;
        data_cv_.notify_one();
    }

    bool pop(std::string& value) {
        std::unique_lock<std::mutex> lock(mutex_);

        data_cv_.wait(lock, [this] {
            return has_value_ || shutdown_;
        });

        if (!has_value_ && shutdown_) return false;

        value = static_cast<std::string&&>(value_);
        value_.clear();
        has_value_ = false;
        free_cv_.notify_one();

        return true;
    }

    void stop() {
        std::lock_guard<std::mutex> lock(mutex_);
        shutdown_ = true;
        data_cv_.notify_all();
        free_cv_.notify_all();
    }

private:
    std::mutex mutex_;
    std::condition_variable data_cv_;
    std::condition_variable free_cv_;
    std::string value_;
    bool has_value_ = false;
    bool shutdown_ = false;
};
