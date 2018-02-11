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
    events_.Push(Event::Type::Scoring, 1);
  }
  level_->Release();
}

void TetrominoSprite::HardDrop() {
  int drop_row = pos_.row();

  pos_ = matrix_->GetDropPosition(pos_, rotation_data_);
  level_->Release();
  floor_reached_ = true;
  last_move_ = Tetromino::Moves::Down;
  events_.Push(Event::Type::Scoring, (kVisibleRows - drop_row) * 2);
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

TetrominoSprite::Status TetrominoSprite::Down(double delta_time) {
  Status status = Status::Continue;

  if (!level_->Wait(delta_time, floor_reached_)) {
    return status;
  }
  if (matrix_->IsValid(Position(pos_.row() + 1, pos_.col()), rotation_data_)) {
    pos_.inc_row();
    matrix_->Insert(pos_, rotation_data_);
    last_move_ = Tetromino::Moves::Down;
    bool floor_is_reached = !matrix_->IsValid(Position(pos_.row() + 1, pos_.col()), rotation_data_);

    if (!floor_reached_ && floor_is_reached) {
      floor_reached_ = true;
      events_.Push(Event::Type::FloorReached);
    } else if (floor_reached_ && !floor_is_reached) {
      level_->ResetTime();
      floor_reached_ = false;
      events_.Push(Event::Type::InTransit);
    }
  } else {
    auto [lines_cleared, tspin_type, perfect_clear] = matrix_->Commit(tetromino_.type(), last_move_, pos_, rotation_data_);

    if (perfect_clear) {
      events_.Push(Event::Type::PerfectClear);
    }
    events_.Push(Event::Type::Scoring, lines_cleared, pos_, tspin_type);
    status = Status::Commited;
  }
  return status;
}
