#pragma once
#include <chrono>

using time_point = std::chrono::time_point<std::chrono::steady_clock>;

class Clock {
public:
  Clock(const Clock&) = delete;
  Clock& operator=(const Clock&) = delete;

  static time_point msc_clock() {
    time_point time = std::chrono::steady_clock::now();
    return time;
  }

  static int64_t elapsed(const std::chrono::time_point<std::chrono::steady_clock>& start, const std::chrono::time_point<std::chrono::steady_clock>& end) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  }
private:
  Clock();
  ~Clock();
};

