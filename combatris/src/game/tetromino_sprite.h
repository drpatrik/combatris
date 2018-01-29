#pragma once

#include "game/coordinates.h"
#include "game/matrix.h"
#include "level.h"

class TetrominoSprite {
 public:
  enum class Rotation { Clockwise, CounterClockwise };

  TetrominoSprite(const Tetromino& tetromino, Level& level, const std::shared_ptr<Matrix>& matrix) : tetromino_(tetromino), angle_(Tetromino::Angle::A0), pos_(0, 5), matrix_(matrix), level_(level), rotation_data_(tetromino.GetRotationData(angle_)) {
    if (matrix_->IsValid(pos_, rotation_data_)) {
      matrix_->Insert(pos_, rotation_data_);
      level_.ResetTime();
    } else {
      matrix_->GetEvents().Push(EventType::GameOver);
    }
  }

  void RotateClockwise();

  void RotateCounterClockwise();

  void SoftDrop();

  void HardDrop();

  void Left();

  void Right();

  void Down(double delta_time);

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
};
