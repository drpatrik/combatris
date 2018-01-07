#pragma once

#include "game/coordinates.h"
#include "game/tetromino.h"

class TetrominoSprite {
 public:
  explicit TetrominoSprite(const Tetromino& tetromino) : tetromino_(tetromino), angle_(Tetromino::Angle::A0), rotation_(tetromino_.GetRotationData(angle_)) {}

  void Rotate() {
    int angle = static_cast<int>(angle_) + 1;

    angle_ = static_cast<Tetromino::Angle>(angle % kAngles.size());
    rotation_ = tetromino_.GetRotationData(angle_);
  }

  void Down() {}

  void Left() {}

  void Right() {}

  void Render();

 protected:
  void RenderGhost();

 private:
  const std::vector<Tetromino::Angle> kAngles = { Tetromino::Angle::A0, Tetromino::Angle::A90, Tetromino::Angle::A180, Tetromino::Angle::A270 };

  const Tetromino& tetromino_;
  Tetromino::Angle angle_;
  TetrominoRotationData rotation_;
  int col_ = 4;
  int row_ = 2;
};
