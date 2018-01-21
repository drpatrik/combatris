#pragma once

#include "game/coordinates.h"
#include "game/matrix.h"
#include "level.h"

class TetrominoSprite {
 public:

  TetrominoSprite(const Tetromino& tetromino, Level& level, const std::shared_ptr<Matrix>& matrix) : tetromino_(tetromino), angle_(Tetromino::Angle::A0), pos_(), matrix_(matrix), level_(level), rotation_data_(tetromino.GetRotationData(angle_)) {
    pos_ = (tetromino_.type() == Tetromino::Type::O) ? Position(0, 6) : Position(0, 5);
    matrix_->Insert(pos_, rotation_data_);
    level_.ResetTime();
  }

  void RotateClockwise();

  void RotateCounterClockwise();

  void SoftDrop() { level_.Release(); }

  void HardDrop();

  void Left();

  void Right();

  // next_piece, lines_cleared, lines_to_movedown
  std::tuple<bool, Matrix::Lines, Matrix::Lines> MoveDown(double delta_time);

 private:
  const std::vector<Tetromino::Angle> kAngles = { Tetromino::Angle::A0, Tetromino::Angle::A90, Tetromino::Angle::A180, Tetromino::Angle::A270 };

  const Tetromino& tetromino_;
  Tetromino::Angle angle_;
  Position pos_;
  std::shared_ptr<Matrix> matrix_;
  Level& level_;
  TetrominoRotationData rotation_data_;
  bool floor_reached_ = false;
};
