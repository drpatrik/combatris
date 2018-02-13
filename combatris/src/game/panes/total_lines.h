#pragma once

#include "game/tetromino_generator.h"

class TotalLines final : public TextPane, public EventSink {
 public:
  TotalLines(SDL_Renderer *renderer, const std::shared_ptr<Assets> &assets)
       : TextPane(renderer, kMatrixStartX - kBlockWidth - (kBoxWidth + 8),
                  (kMatrixStartY - kBlockHeight) + 578, "LINES", assets) { Reset(); }

  virtual void Reset() override { total_lines_ = 0;  SetCenteredText(std::to_string(0)); }

  virtual void Update(const Event& event) override {
    if (Event::Type::ScoringData == event.type() && !event.IsDrop()) {
      total_lines_ += event.lines_cleared();
      SetCenteredText(total_lines_);
    }
  }

 private:
  int total_lines_ = 0;
};
