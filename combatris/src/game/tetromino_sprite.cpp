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

void TetrominoSprite::RotateRight() {
    int angle = static_cast<int>(angle_) + 1;

  angle_ = static_cast<Tetromino::Angle>(angle % kAngles.size());
  rotation_ = tetromino_.GetRotationData(angle_);
}

void TetrominoSprite::RotateLeft() {}

void TetrominoSprite::HardDrop() {}

void TetrominoSprite::SoftDrop() {}

void TetrominoSprite::Left() {}

void TetrominoSprite::Right() {}
