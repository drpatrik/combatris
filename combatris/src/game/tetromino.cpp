#include "game/tetromino.h"

void Tetromino::Render(int x, int y) const {
  SDL_Rect dest_rc {x, y, kBlockWidth, kBlockHeight };

  SDL_RenderCopy(renderer_, tetromino_.get(), nullptr, &dest_rc);
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

void Tetromino::RenderOutline(int x, int y, Angle angle) const {
  const auto& metadata = metadata_.at(static_cast<int>(angle));

  for (size_t row = 0; row < metadata.shape_.size(); ++row) {
    int t_x = x;
    for (size_t col = 0; col < metadata.shape_.at(row).size(); ++col) {
      const auto& shape = metadata.shape_;

      if (shape[row][col] != 0) {
        SDL_Rect rc { t_x, y, kBlockWidth, kBlockHeight };

        SDL_SetRenderDrawColor(renderer_, metadata.color_.r, metadata.color_.g, metadata.color_.b, metadata.color_.a);
        SDL_RenderFillRect(renderer_, &rc);

        rc = { t_x + 1, y + 1, kBlockWidth - 2, kBlockHeight - 2 };

        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0);
        SDL_RenderFillRect(renderer_, &rc);
      }
      t_x += kBlockWidth;
    }
    y += kBlockHeight;
  }
}
