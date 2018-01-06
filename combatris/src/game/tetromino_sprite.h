#pragma once

#include "game/coordinates.h"
#include "game/tetromino.h"

class TetrominoSprite {
 public:
  explicit TetrominoSprite(const Tetromino& tetromino) : tetromino_(tetromino), angle_(Tetromino::Angle::A0) {}

  void Rotate() {
    int angle = static_cast<int>(angle_) + 1;

    angle_ = static_cast<Tetromino::Angle>(angle % kAngles.size());
  }

  void Down() {}

  void Left() {}

  void Right() {}

  void Render(int row, int col) const { tetromino_.Render(col_to_pixel(col), row_to_pixel(row), angle_); }

  void RenderOutline(int row, int col) const { tetromino_.RenderOutline(col_to_pixel(col), row_to_pixel(row), angle_); }

 private:
  const std::vector<Tetromino::Angle> kAngles = {Tetromino::Angle::A0, Tetromino::Angle::A90, Tetromino::Angle::A180, Tetromino::Angle::A270 };

  const Tetromino& tetromino_;
  Tetromino::Angle angle_;
};
