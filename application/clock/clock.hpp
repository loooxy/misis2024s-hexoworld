#pragma once
#include <chrono>

using time_point = std::chrono::time_point<std::chrono::steady_clock>;

auto msc_clock() {
  auto time = std::chrono::steady_clock::now();
  return time;
}

int64_t elapsed(const std::chrono::time_point<std::chrono::steady_clock>& start, const std::chrono::time_point<std::chrono::steady_clock>& end) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}