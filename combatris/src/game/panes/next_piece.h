#pragma once

#include "game/tetromino_generator.h"
#include "game/panes/pane.h"

class NextPiece final : public TextPane {
 public:
  NextPiece(SDL_Renderer* renderer, const std::shared_ptr<TetrominoGenerator>& tetromino_generator, const std::shared_ptr<Assets>& assets)
      : TextPane(renderer,  kMatrixEndX + kBlockWidth + 8,
                 kMatrixStartY - kBlockHeight, "NEXT", assets), tetromino_generator_(tetromino_generator) {
    SetCaptionOrientation(TextPane::Orientation::Left);
  }

  void Hide() { hide_pieces_ = true; }

  void Show() { hide_pieces_ = false; }

  virtual void Render() const override {
    TextPane::Render();
    if (hide_pieces_) {
      return;
    }
    tetromino_generator_->RenderFromQueue(0, x_ + 10, y_ + caption_height_ + 15);

    for (size_t i = 1; i < 3; ++i) {
      tetromino_generator_->RenderFromQueue(i, x_ + 10, y_ + caption_height_ + 15 + (90 * i));
    }
  }

  virtual void Reset() override {}

 private:
  bool hide_pieces_ = true;
  const std::shared_ptr<TetrominoGenerator> tetromino_generator_;
};
