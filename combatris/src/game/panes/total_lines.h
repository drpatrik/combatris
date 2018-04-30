#pragma once

#include "game/tetromino_generator.h"

class TotalLines final : public TextPane, public EventListener {
 public:
  TotalLines(SDL_Renderer* renderer, const std::shared_ptr<Assets>& assets)
       : TextPane(renderer, kMatrixStartX - kMinoWidth - (kBoxWidth + kSpace),
                  (kMatrixStartY - kMinoHeight) + 578, "LINES", assets) { Reset(); }

  virtual void Reset() override { total_lines_ = 0;  SetCenteredText(std::to_string(0)); }

  virtual void Update(const Event& event) override {
    if (!event.Is(Event::Type::LinesCleared)) {
      return;
    }
    total_lines_ += event.lines_cleared();
    SetCenteredText(total_lines_);
  }

 private:
  int total_lines_ = 0;
};
