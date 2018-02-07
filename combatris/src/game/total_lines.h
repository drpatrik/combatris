#pragma once

#include "game/pane.h"
#include "game/tetromino_generator.h"

class TotalLines final : public TextPane {
 public:
  TotalLines(SDL_Renderer *renderer, const std::shared_ptr<Assets> &assets)
       : TextPane(renderer, kMatrixStartX - kBlockWidth - 165,
                  (kMatrixStartY - kBlockHeight) + 578, "LINES", assets) { NewGame(); }

  void NewGame() { total_lines_ = 0;  SetCenteredText(std::to_string(0)); }

  void Update(const Event& event) {
    if (Event::Type::Scoring == event.type() && !event.IsDrop()) {
      total_lines_ += event.lines_cleared();
      SetCenteredText(total_lines_);
    }
  }

 private:
  int total_lines_ = 0;
};
