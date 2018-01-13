#pragma once

#include "game/coordinates.h"
#include "game/tetromino.h"

class TetrominoSprite {
 public:
  explicit TetrominoSprite(const Tetromino& tetromino) : tetromino_(tetromino), angle_(Tetromino::Angle::A0), rotation_(tetromino_.GetRotationData(angle_)), pos_(0, 0) {}

  void RotateRight();

  void RotateLeft();

  void HardDrop();

  void SoftDrop();

  void Left();

  void Right();

 private:
  const std::vector<Tetromino::Angle> kAngles = { Tetromino::Angle::A0, Tetromino::Angle::A90, Tetromino::Angle::A180, Tetromino::Angle::A270 };

  const Tetromino& tetromino_;
  Tetromino::Angle angle_;
  TetrominoRotationData rotation_;
  Position pos_;
};
