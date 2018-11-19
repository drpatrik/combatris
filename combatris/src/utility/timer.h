#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <iostream>

namespace utility {

inline int64_t time_in_ms() {
  auto current_time = std::chrono::high_resolution_clock::now();

  return std::chrono::duration_cast<std::chrono::milliseconds>(current_time.time_since_epoch()).count();
}

std::string FormatTimeMMSSHS(size_t t);

class TimerInterface {
 public:
  virtual ~TimerInterface() {};

  virtual std::pair<bool, size_t> GetTime() = 0;

  virtual void Set(int value) = 0;

  virtual void Start() = 0;

  virtual void Stop() = 0;

  virtual void TogglePause()  = 0;

  virtual bool IsStarted() const = 0;

  virtual void Reset() = 0;

  virtual bool IsZero() = 0;

  virtual std::string FormatTime(size_t t) const = 0;
};

class Timer final : public TimerInterface {
 public:
  using SystemClock = std::chrono::system_clock;
  using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

  explicit Timer(int value) : initial_value_(value), count_down_(value), start_(SystemClock::now()) {}

  virtual std::pair<bool, size_t> GetTime() override {
    if (paused_) {
      return std::make_pair(false, count_down_);
    }
    if (std::chrono::duration_cast<std::chrono::milliseconds>(SystemClock::now() - start_).count() >= 1000) {
      start_ = SystemClock::now();
      --count_down_;
      count_down_ = std::max(count_down_, 0);
      return std::make_pair(true, count_down_);
    }
    return std::make_pair(false, count_down_);
  }

  virtual void Set(int value) override {
    Stop();
    initial_value_ = value;
  }

  virtual void Start() override {
    Reset();
    timer_started_ = true;
  }

  virtual void Stop() override { timer_started_ = false; }

  virtual void TogglePause() override {
    paused_ = !paused_;
    if (!paused_) {
      start_ = SystemClock::now();
    }
  }

  virtual bool IsStarted() const override { return timer_started_; }

  virtual void Reset() override {
    paused_ = false;
    timer_started_ = false;
    count_down_ = initial_value_;
    start_ = SystemClock::now();
  }

  virtual bool IsZero() override { return GetTime().second == 0; }

  virtual std::string FormatTime(size_t t) const override;

 private:
  int initial_value_;
  int count_down_;
  TimePoint start_;
  bool timer_started_ = false;
  bool paused_ = false;
};

class Clock final : public TimerInterface {
 public:
  using SystemClock = std::chrono::system_clock;
  using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

  Clock() : ms_(0), start_(SystemClock::now()) {}

  virtual std::pair<bool, size_t> GetTime() override {
    if (paused_) {
      return std::make_pair(false, ms_);
    }
    auto d = std::chrono::duration_cast<std::chrono::milliseconds>(SystemClock::now() - start_).count();

    if (d >= 10) {
      ms_ += d;
      start_ = SystemClock::now();
      return std::make_pair(true, ms_);
    }
    return std::make_pair(false, ms_);
  }

  virtual void Set(int) override {}

  virtual void Start() override {
    Reset();
    timer_started_ = true;
  }

  virtual void Stop() override { timer_started_ = false; }

  virtual void TogglePause() override {
    paused_ = !paused_;
    if (!paused_) {
      start_ = SystemClock::now();
    }
  }

  virtual bool IsStarted() const override { return timer_started_; }

  virtual void Reset() override {
    paused_ = false;
    timer_started_ = false;
    ms_ = 0;
    start_ = SystemClock::now();
  }

  virtual bool IsZero() override { return false; }

  virtual std::string FormatTime(size_t t) const override;

 private:
  size_t ms_;
  TimePoint start_;
  bool timer_started_ = false;
  bool paused_ = false;
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
