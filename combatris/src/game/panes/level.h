#pragma once

#include "game/events.h"
#include "game/panes/pane.h"

class Level final : public TextPane, public EventSink {
 public:
   Level(SDL_Renderer* renderer, Events& events, const std::shared_ptr<Assets>& assets)
       : TextPane(renderer, kMatrixStartX - kBlockWidth - (kBoxWidth + 8), (kMatrixStartY - kBlockHeight) + 428, "LEVEL", assets),
         events_(events) { SetCenteredText(1); SetThresholds(); }

   bool Wait(double time_delta, bool floor_reached) {
     return (floor_reached) ? WaitForLockDelay(time_delta)
                            : WaitForMoveDown(time_delta); }

  void Release() { time_ += 60.0; }

  virtual void Update(const Event& event) override;

  virtual void Reset() override {
    time_ = 0.0;
    level_ = 0;
    total_lines_ = 0;
    lines_this_level_ = 0;
    SetThresholds();
    SetCenteredText(1);
  }

  inline int level() const { return level_ + 1; }

  inline double lock_delay() const { return lock_delay_; }

  Events& GetEvents() { return events_; }

  void SetThresholds();

  inline void ResetTime() { time_ = 0.0; }

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
