/**
 * @file ConcurrentQueue.h
 * @author Ilija Poznic
 * @date 2025
 */

#pragma once

#include <queue>
#include <mutex>
#include <shared_mutex>
#include <thread>

namespace {
    constexpr auto wait_time = std::chrono::milliseconds(1);

    std::unique_lock<std::shared_mutex> lock(std::shared_mutex &mutex) {
        while (true) {
            try {
                std::unique_lock<std::shared_mutex> lock(mutex);
                return std::move(lock);
            } catch (...) {
            }
            std::this_thread::sleep_for(wait_time);
        }
    }
}

namespace pie::concurrent {
    template<typename Value>
    class ConcurrentQueue {
    public:
        void push(const Value &value) {
            auto locker = lock(mutex);
            queue.push(value);
        }

        Value pop() {
            auto locker = lock(mutex);
            auto value = queue.front();
            queue.pop();
            return value;
        }

        bool empty() {
            auto locker = lock(mutex);
            return queue.empty();
        }

    private:
        std::queue<Value> queue{};
        std::shared_mutex mutex{};
    };
}
