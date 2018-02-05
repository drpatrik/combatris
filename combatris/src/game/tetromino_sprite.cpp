#include "game/tetromino_sprite.h"

#include <unordered_map>

namespace {

using Angle = Tetromino::Angle;
using Rotation = TetrominoSprite::Rotation;

const std::unordered_map<Tetromino::Angle, const std::unordered_map<Tetromino::Angle, int>> kStates = {
  { Angle::A0, { { Angle::A90, 0 }, { Angle::A270, 7 } } },
  { Angle::A90, { { Angle::A180, 2 }, { Angle::A0, 1 } } },
  { Angle::A180, { { Angle::A270, 4 }, { Angle::A90, 3 } } },
  { Angle::A270, { { Angle::A0, 6 }, { Angle::A180 , 5 } } }
};

inline const std::vector<std::vector<int>>& GetWallKickData(Tetromino::Type type, Tetromino::Angle from_angle, Tetromino::Angle to_angle) {
  int state = kStates.at(from_angle).at(to_angle);

  return (type == Tetromino::Type::I) ? kWallKickDataForI[state] : kWallKickDataForJLSTZ[state];
}

Tetromino::Angle GetNextAngle(Tetromino::Angle current_angle, Rotation rotate) {
  auto angle= static_cast<int>(current_angle);

  angle += (rotate == Rotation::Clockwise) ? 1 : -1;
  if (angle > static_cast<int>(Tetromino::Angle::A270)) {
    angle = 0;
  } else if (angle < 0) {
    angle =  static_cast<int>(Tetromino::Angle::A270);
  }
  return static_cast<Tetromino::Angle>(angle);
}

} // namespace

std::tuple<bool, Position, Tetromino::Angle> TetrominoSprite::TryRotation(Tetromino::Type type, const Position& current_pos, Tetromino::Angle current_angle, Rotation rotate) {
  enum { GetX = 0, GetY = 1 };

  auto try_angle = GetNextAngle(current_angle, rotate);
  const auto& wallkick_data = GetWallKickData(type, current_angle, try_angle);

  for (const auto& offsets : wallkick_data) {
    Position try_pos(current_pos.row() + offsets[GetY], current_pos.col() + offsets[GetX]);

    if (matrix_->IsValid(try_pos, tetromino_.GetRotationData(try_angle))) {
      return std::make_tuple(true, try_pos, try_angle);
    }
  }
  return std::make_tuple(false, current_pos, current_angle);
}

void TetrominoSprite::RotateClockwise() {
  bool success;

  std::tie(success, pos_, angle_) = TryRotation(tetromino_.type(), pos_, angle_, Rotation::Clockwise);

  if (success) {
    rotation_data_ = tetromino_.GetRotationData(angle_);
    matrix_->Insert(pos_, rotation_data_);
    last_move_ = Tetromino::Moves::Rotation;
  }
}

void TetrominoSprite::RotateCounterClockwise() {
  bool success;

  std::tie(success, pos_, angle_) = TryRotation(tetromino_.type(), pos_, angle_, Rotation::CounterClockwise);

  if (success) {
    rotation_data_ = tetromino_.GetRotationData(angle_);
    matrix_->Insert(pos_, rotation_data_);
    last_move_ = Tetromino::Moves::Rotation;
  }
}

void TetrominoSprite::SoftDrop() {
  if (floor_reached_) {
    return;
  }
  if (pos_.row() >= kVisibleRowStart - 1) {
    matrix_->GetEvents().Push(Event::Type::SoftDrop, 1);
  }
  level_.Release();
}

void TetrominoSprite::HardDrop() {
  int drop_row = pos_.row();

  pos_ = matrix_->GetDropPosition(pos_, rotation_data_);
  level_.Release();
  floor_reached_ = true;
  matrix_->GetEvents().Push(Event::Type::HardDrop, kVisibleRows - drop_row);
}

void TetrominoSprite::Left() {
  if (matrix_->IsValid(Position(pos_.row(), pos_.col() - 1), rotation_data_)) {
    pos_.dec_col();
    matrix_->Insert(pos_, rotation_data_);
    last_move_ = Tetromino::Moves::Left;
  }
}

void TetrominoSprite::Right() {
  if (matrix_->IsValid(Position(pos_.row(), pos_.col() + 1), rotation_data_)) {
    pos_.inc_col();
    matrix_->Insert(pos_, rotation_data_);
    last_move_ = Tetromino::Moves::Right;
  }
}

void TetrominoSprite::Down(double delta_time) {
  if (!level_.Wait(delta_time, floor_reached_)) {
    return;
  }
  if (floor_reached_) {
    matrix_->Commit(tetromino_.type(), last_move_, pos_, rotation_data_);
    return;
  }
  if (matrix_->IsValid(Position(pos_.row() + 1, pos_.col()), rotation_data_)) {
    pos_.inc_row();
    matrix_->Insert(pos_, rotation_data_);
    last_move_ = Tetromino::Moves::Down;
    if (!matrix_->IsValid(Position(pos_.row() + 1, pos_.col()), rotation_data_)) {
      floor_reached_ = true;
      matrix_->GetEvents().Push(Event::Type::FloorReached);
    }
  } else {
    matrix_->GetEvents().Push(Event::Type::GameOver);
  }
}
