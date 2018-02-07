#pragma once

#include "game/pane.h"
#include "game/events.h"

class Level final : public TextPane {
 public:
  Level(SDL_Renderer* renderer, Events& events, const std::shared_ptr<Assets>& assets)
      : TextPane(renderer, kMatrixStartX - kBlockWidth - 165, (kMatrixStartY - kBlockHeight) + 428, "LEVEL", assets), events_(events) {
    ResetTime();
    SetCenteredText(1);
  }

  bool Wait(double time_delta, bool floor_reached) { return (floor_reached) ? WaitForLockDelay(time_delta) : WaitForMoveDown(time_delta); }

  void Release() { time_ += 60.0; }

  void Update(const Event& event);

  int level() const { return level_ + 1; }

  void NewGame() {
    time_ = 0.0;
    wait_time_ = 0.0;
    lock_delay_ = 0.0;
    level_ = 0;
    total_lines_ = 0;
    lines_this_level_ = 0;
    SetCenteredText(1);
  }

  void ResetTime();

 protected:
  bool WaitForMoveDown(double time_delta);

  bool WaitForLockDelay(double time_delta);

 private:
  Events& events_;
  double time_ = 0.0;
  double wait_time_ = 0.0;
  double lock_delay_ = 0.0;
  int level_ = 0;
  int total_lines_ = 0;
  int lines_this_level_ = 0;
};
