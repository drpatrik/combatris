
#pragma once

#include "game/coordinates.h"
#include "game/matrix.h"
#include "level.h"

class TetrominoSprite {
 public:
  enum class Rotation { Clockwise, CounterClockwise };

  TetrominoSprite(const Tetromino& tetromino, Level& level, const std::shared_ptr<Matrix>& matrix) : tetromino_(tetromino), angle_(Tetromino::Angle::A0), pos_(), matrix_(matrix), level_(level), rotation_data_(tetromino.GetRotationData(angle_)) {
    pos_ = (tetromino_.type() == Tetromino::Type::O) ? Position(0, 6) : Position(0, 5);
    if (matrix_->IsValid(pos_,  rotation_data_)) {
      matrix_->Insert(pos_, rotation_data_);
      can_move_ = true;
    }
    level_.ResetTime();
  }

  void RotateClockwise();

  void RotateCounterClockwise();

  void SoftDrop() {
    if (!floor_reached_) {
      level_.Release();
    }
  }

  void HardDrop();

  void Left();

  void Right();

  std::pair<bool, Matrix::Lines> Down(double delta_time);

  bool CanMove() const { return can_move_; }

 protected:
  std::tuple<bool, Position, Tetromino::Angle> TryRotation(Tetromino::Type type, const Position& current_pos, Tetromino::Angle current_angle, Rotation rotatet);

 private:
  const std::vector<Tetromino::Angle> kAngles = { Tetromino::Angle::A0, Tetromino::Angle::A90, Tetromino::Angle::A180, Tetromino::Angle::A270 };

  const Tetromino& tetromino_;
  Tetromino::Angle angle_;
  Position pos_;
  std::shared_ptr<Matrix> matrix_;
  Level& level_;
  TetrominoRotationData rotation_data_;
  bool floor_reached_ = false;
  bool can_move_ = false;
};
