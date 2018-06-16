#pragma once

#include "game/panes/level.h"

class Goal final : public TextPane, public EventListener {
 public:
  // 578
  Goal(SDL_Renderer* renderer, int offset, const std::shared_ptr<Assets>& assets) : TextPane(renderer, kMatrixStartX - kMinoWidth - (kBoxWidth + kSpace), (kMatrixStartY - kMinoHeight) + offset, "GOAL", assets) { Reset(); }

  virtual void Reset() override {
    level_ = start_level_;
    goal_ = level_ * 5;
    SetCenteredText(goal_);
  }

  virtual void Update(const Event& event) override {
    if (!IsIn(event, { Event::Type::LinesCleared, Event::Type::LevelUp, Event::Type::SetStartLevel })) {
      return;
    }
    switch (event) {
      case Event::Type::SetStartLevel:
        level_ = start_level_ = event.value_;
        goal_ = level_ * 5;
        std::cout << "current goal " << goal_ << std::endl;
        break;
      case Event::Type::LinesCleared:
        goal_ = std::max(goal_ - event.value_, 0);
        break;
      case Event::Type::LevelUp:
        level_ = event.value_;
        goal_ = level_ * 5;
        std::cout << "New Goal: " << goal_ << std::endl;
        break;
      default:
        break;
    }
    SetCenteredText(goal_);
  }

 private:
  int goal_ = 0;
  int level_ = 1;
  int start_level_ = 1;
};
