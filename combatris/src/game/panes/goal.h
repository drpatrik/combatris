#pragma once

#include "game/panes/level.h"

class Goal final : public TextPane, public EventListener {
 public:
  // 578
  Goal(SDL_Renderer* renderer, int offset, const std::shared_ptr<Assets>& assets, const std::shared_ptr<Level>& level) : TextPane(renderer, kMatrixStartX - kMinoWidth - (kBoxWidth + kSpace), (kMatrixStartY - kMinoHeight) + offset, "GOAL", assets), level_(level) { Reset(); }

  virtual void Reset() override {
    goal_ = level_->level() * 5;
    level_->SetLinesForNextLevel(goal_);
    SetCenteredText(std::to_string(goal_));
  }

  virtual void Update(const Event& event) override {
    if (!IsIn(event, { Event::Type::LinesCleared, Event::Type::LevelUp })) {
      return;
    }
    if (Event::Type::LinesCleared == event) {
      goal_ = std::max(goal_ - event.value_, 0);
      std::cout << goal_ << std::endl;
    } else {
      goal_ = level_->level() * 5;
      level_->SetLinesForNextLevel(goal_);
      std::cout << "New Goal: " << goal_ << std::endl;
    }
    SetCenteredText(goal_);
  }

 private:
  int goal_ = 0;
  std::shared_ptr<Level> level_;
};
