#pragma once

#include "game/pane.h"
#include "game/tetromino_generator.h"

class NextPiece final : public Pane {
 public:
  NextPiece(SDL_Renderer* renderer, const std::shared_ptr<TetrominoGenerator>& tetromino_generator, const std::shared_ptr<Assets>& assets)
      : Pane(renderer, 750, 10, assets), tetromino_generator_(tetromino_generator) {}

  void Hide() { hide_pieces_ = true; }

  void Show() { hide_pieces_ = false; }

  virtual void Render() const override {
    if (hide_pieces_) {
      return;
    }
    RenderText(x_, y_ , Font::Normal, "Next: ", Color::White);
    for (size_t i = 0; i < 3; ++i) {
      tetromino_generator_->RenderFromQueue(i, x_, y_ + 40 + (100 * i));
    }
  }

 private:
  bool hide_pieces_ = true;
  const std::shared_ptr<TetrominoGenerator> tetromino_generator_;
};
