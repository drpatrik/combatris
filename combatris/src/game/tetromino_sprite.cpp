#include "game/tetromino_sprite.h"

/*void TetrominoSprite::Render() { // Just for test
  int y = pos_.y();

  for (size_t row = 0; row < rotation_.data_.size(); ++row) {
    int t_x = pos_.x();

    for (size_t col  = 0; col < rotation_.data_.at(row).size(); ++col) {
      const auto& data = rotation_.data_;

      if (data[row][col] != 0) {
        tetromino_.Render(t_x, y);
      }
      t_x += kBlockWidth;

    }
    y += kBlockHeight;
  }
  }*/

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
