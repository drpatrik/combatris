#pragma once

#include <string>
#include <chrono>
#include <algorithm>

namespace utility {

inline int64_t time_in_ms() {
  auto current_time = std::chrono::high_resolution_clock::now();

  return std::chrono::duration_cast<std::chrono::milliseconds>(current_time.time_since_epoch()).count();
}

class Timer final {
 public:
  using SystemClock = std::chrono::system_clock;
  using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

  explicit Timer(int value) : initial_value_(value), count_down_(value), start_(SystemClock::now()) {}

  operator int() { return GetTimeInSeconds().second; }

  std::pair<bool, int> GetTimeInSeconds() {
    if (std::chrono::duration_cast<std::chrono::milliseconds>(SystemClock::now() - start_).count() >= 1000) {
      start_ = SystemClock::now();
      --count_down_;
      count_down_ = std::max(count_down_, 0);
      return std::make_pair(true, count_down_);
    }
    return std::make_pair(false, count_down_);
  }

  void Start() {
    Reset();
    timer_started_ = true;
  }

  void Stop() { timer_started_ = false; }

  inline bool IsStarted() const { return timer_started_; }

  std::string FormatTime(size_t seconds) const;

  std::string FormatTime() { return FormatTime(GetTimeInSeconds().second); }

  void Reset() {
    timer_started_ = false;
    count_down_ = initial_value_;
    start_ = SystemClock::now();
  }

  bool IsZero() { return GetTimeInSeconds().second == 0; }

 private:
  int initial_value_;
  int count_down_;
  TimePoint start_;
  bool timer_started_ = false;
};

// Adapted from http://headerphile.com/sdl2/sdl2-part-9-no-more-delays/
class DeltaTimer final {
public:
  using HighResClock = std::chrono::high_resolution_clock;
  using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

  DeltaTimer() : previous_time_(HighResClock::now()) {}

  // Returns time since last time this function was called
  // in seconds with nanosecond precision
  double GetDelta() {
    // 1. Get current time as a std::chrono::time_point
    auto current_time = HighResClock::now();

    // 2. Get the time difference as seconds
    // ...represented as a double
    std::chrono::duration<double> delta{ current_time - previous_time_ };

    // 3. Reset the timePrev to the current point in time
    previous_time_ = HighResClock::now();

    // 4. Returns the number of ticks in delta
    return delta.count();
  }

  void Reset() { previous_time_ = HighResClock::now(); }

private:
  TimePoint previous_time_;
};

} // namespace utility
