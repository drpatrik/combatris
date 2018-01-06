#include "game/tetromino.h"

#include <iostream>

void Tetromino::Render(int x, int y) const {
  int offset = metadata_.at(0).renderer_hint_ * kBlockHeight;

  SDL_Rect src_rc {0, offset, kBlockWidth, kBlockHeight };
  SDL_Rect dest_rc {x, y, kBlockWidth, kBlockHeight };

  SDL_RenderCopy(renderer_, tetromino_.get(), &src_rc, &dest_rc);
}

void Tetromino::Render(int x, int y, Angle angle) const {
  const auto& metadata = metadata_.at(static_cast<int>(angle));

  for (size_t row = 0; row < metadata.shape_.size(); ++row) {
    int t_x = x;
    for (size_t col = 0; col < metadata.shape_.at(row).size(); ++col) {
      const auto& shape = metadata.shape_;

      if (shape[row][col] != 0) {
        Render(t_x, y);
      }
      t_x += kBlockWidth;
    }
    y += kBlockHeight;
  }
}
