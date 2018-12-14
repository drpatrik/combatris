#pragma once

#include "game/events.h"
#include "game/panes/pane.h"

class TotalLines final : public TextPane, public EventListener {
 public:
  // 578
  TotalLines(SDL_Renderer* renderer, int offset, const std::shared_ptr<Assets>& assets)
       : TextPane(renderer, kMatrixStartX - kMinoWidth - (kBoxWidth + kSpace),
                  (kMatrixStartY - kMinoHeight) + offset, "LINES", assets) { Reset(); }

  virtual void Reset() override { total_lines_ = 0;  SetCenteredText(std::to_string(0)); }

  virtual void Update(const Event& event) override {
    if (!event.Is(Event::Type::LinesCleared)) {
      return;
    }
    total_lines_ += event.value1_;
    SetCenteredText(total_lines_);
  }

 private:
  int total_lines_ = 0;
};
