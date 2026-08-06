#include "common/utils/Timer.hpp"

void Timer::start() noexcept {
    start_ = std::chrono::steady_clock::now();
}

double Timer::elapsed_seconds() const noexcept {
    const auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(end - start_).count();
}

