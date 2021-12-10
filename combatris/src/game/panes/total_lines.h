#pragma once

#include "game/events.h"
#include "game/panes/pane.h"

class TotalLines final : public TextPane, public EventListener {
 public:
  static const int kYOffs = 578;

  TotalLines(SDL_Renderer* renderer, const std::shared_ptr<Assets>& assets)
       : TextPane(renderer, kMatrixStartX - kMinoWidth - (kBoxWidth + kSpace),
                  (kMatrixStartY - kMinoHeight) + kYOffs, "LINES", assets) { SetCenteredText(std::to_string(0)); }

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
