#include "game/tetromino_sprite.h"

void TetrominoSprite::Render() { // Just for test
  int y = row_to_pixel(row_);

  for (size_t row = 0; row < rotation_.data_.size(); ++row) {
    int t_x = col_to_pixel(col_);

    for (size_t col  = 0; col < rotation_.data_.at(row).size(); ++col) {
      const auto& data = rotation_.data_;

      if (data[row][col] != 0) {
        tetromino_.RenderXY(t_x, y);
      }
      t_x += kBlockWidth;

    }
    y += kBlockHeight;
  }
}

void TetrominoSprite::RenderGhost() {}
