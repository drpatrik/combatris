#pragma once

#include "game/events.h"
#include "game/panes/pane.h"

class Level final : public TextPane, public EventListener {
 public:
  enum class LinesForNextLevelMode { Normal, Marathon };
  Level(SDL_Renderer* renderer, int offset, Events& events, const std::shared_ptr<Assets>& assets)
      : TextPane(renderer, kMatrixStartX - kMinoWidth - (kBoxWidth + kSpace), (kMatrixStartY - kMinoHeight) + offset, "LEVEL", assets),
        events_(events) { SetCenteredText(1); SetThresholds(); }

  bool WaitForMoveDown(double time_delta);

  bool WaitForLockDelay(double time_delta);

  bool WaitForLockDelay() { return WaitForLockDelay(0); }

  inline void Release() { time_ += 60.0; }

  virtual void Update(const Event& event) override;

  virtual void Reset() override {
    time_ = 0.0;
    total_lines_ = 0;
    lines_this_level_ = 0;
    SetLevel(start_level_);
  }

  inline int level() const { return level_; }

  inline void ResetTime() { time_ = 0.0; }

 protected:
  void SetThresholds();
  void SetLevel(int lvl);

 private:
  Events& events_;
  double time_ = 0.0;
  double wait_time_ = 0.0;
  double lock_delay_ = 0.0;
  int total_lines_ = 0;
  int lines_this_level_ = 0;
  int lines_for_next_level_ = 0;
  int level_ = 1;
  int start_level_ = 1;
  CampaignType campaign_type_ = CampaignType::Combatris;
};
