#pragma once

#include <nett_common.h>

namespace nett {

    template<typename T>
    class ts_queue {
    public:
        ts_queue() = default;
        ts_queue(const ts_queue&) = delete;
        virtual ~ts_queue() { clear(); }

        auto front() -> const T& {
            std::scoped_lock lock(muxQueue);
            return deqQueue.front();
        }

        auto back() -> const T& {
            std::scoped_lock lock(muxQueue);
            return deqQueue.back();
        }

        auto push_back(const T& item) -> void {
            std::scoped_lock lock(muxQueue);
            deqQueue.emplace_back(std::move(item));
        }

        auto empty() -> bool {
            std::scoped_lock lock(muxQueue);
            return deqQueue.empty();
        }

        auto count() -> size_t {
            std::scoped_lock lock(muxQueue);
            return deqQueue.size();
        }

        auto clear() -> void {
            std::scoped_lock lock(muxQueue);
            deqQueue.clear();
        }

        auto pop_front() -> T {
            std::scoped_lock lock(muxQueue);
            auto t = std::move(deqQueue.front());
            deqQueue.pop_front();
            return t;
        }

        auto pop_back() -> T {
            std::scoped_lock lock(muxQueue);
            auto t = std::move(deqQueue.back());
            deqQueue.pop_back();
            return t;
        }

    protected:
        std::mutex muxQueue;
        std::deque<T> deqQueue;
    };

} // namespace nett