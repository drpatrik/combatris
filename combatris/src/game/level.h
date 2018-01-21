#pragma once

class Level {
 public:
  Level() { ResetTime(); }

  bool Wait(double time_delta, bool floor_reached) { return (floor_reached) ? WaitForLockDelay(time_delta) : WaitForMoveDown(time_delta); }

  void Release() { time_ += 60.0; }

  int GetScore(int lines_cleared);

  void Reset() {
    time_ = 0.0;
    wait_time_ = 0.0;
    lock_delay_ = 0.0;
    level_ = 0;
    lines_this_level_ = 0;
  }

  void ResetTime();

 protected:
  bool WaitForMoveDown(double time_delta);

  bool WaitForLockDelay(double time_delta);

 private:
  double time_ = 0.0;
  double wait_time_ = 0.0;
  double lock_delay_ = 0.0;
  int level_ = 0;
  int lines_this_level_ = 0;
};
