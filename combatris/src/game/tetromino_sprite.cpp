#include "game/tetromino_sprite.h"

#include <unordered_map>

namespace {

const int kResetsAllowed = 15;

using Angle = Tetromino::Angle;
using Rotation = TetrominoSprite::Rotation;

const std::unordered_map<Tetromino::Angle, const std::unordered_map<Tetromino::Angle, int>> kStates = {
  { Angle::A0, { { Angle::A90, 0 }, { Angle::A270, 7 } } },
  { Angle::A90, { { Angle::A180, 2 }, { Angle::A0, 1 } } },
  { Angle::A180, { { Angle::A270, 4 }, { Angle::A90, 3 } } },
  { Angle::A270, { { Angle::A0, 6 }, { Angle::A180 , 5 } } }
};

inline const std::vector<std::vector<int>>& GetWallKickData(Tetromino::Type type, Tetromino::Angle from_angle, Tetromino::Angle to_angle) {
  auto state = kStates.at(from_angle).at(to_angle);

  return (Tetromino::Type::I == type) ? kWallKickDataForI[state] : kWallKickDataForJLSTZ[state];
}

Tetromino::Angle GetNextAngle(Tetromino::Angle current_angle, Rotation rotate) {
  auto angle= static_cast<int>(current_angle);

  angle += (Rotation::Clockwise == rotate) ? 1 : -1;
  if (angle > static_cast<int>(Tetromino::Angle::A270)) {
    angle = 0;
  } else if (angle < 0) {
    angle =  static_cast<int>(Tetromino::Angle::A270);
  }
  return static_cast<Tetromino::Angle>(angle);
}

} // namespace

void TetrominoSprite::ResetDelayCounter() {
  if (State::OnFloor == state_ && reset_delay_counter_ < kResetsAllowed) {
    ++reset_delay_counter_;
    level_->ResetTime();
  } else if (reset_delay_counter_ > 0) {
    ++reset_delay_counter_;
  }
}

opt::optional<std::pair<Position, Tetromino::Angle>> TetrominoSprite::TryRotation(Tetromino::Type type, const Position& current_pos, Tetromino::Angle current_angle, Rotation rotate) {
  if (Tetromino::Type::O == type) {
    return {};
  }
  enum { GetX = 0, GetY = 1 };

  auto try_angle = GetNextAngle(current_angle, rotate);

  const auto& rotation_data = tetromino_.GetRotationData(try_angle);
  const auto& wallkick_data = GetWallKickData(type, current_angle, try_angle);

  for (const auto& offsets : wallkick_data) {
    Position try_pos(current_pos.row() + offsets[GetY], current_pos.col() + offsets[GetX]);

    if (matrix_->IsValid(try_pos, rotation_data)) {
      ResetDelayCounter();
      return opt::make_optional(std::make_pair(try_pos, try_angle));
    }
  }
  return {};
}

void TetrominoSprite::RotateClockwise() {
  if (auto result = TryRotation(tetromino_.type(), pos_, angle_, Rotation::Clockwise)) {
    std::tie(pos_, angle_) = *result;
    rotation_data_ = tetromino_.GetRotationData(angle_);
    matrix_->Insert(pos_, rotation_data_);
    last_move_ = Tetromino::Move::Rotation;
  }
}

void TetrominoSprite::RotateCounterClockwise() {
  if (auto result = TryRotation(tetromino_.type(), pos_, angle_, Rotation::CounterClockwise)) {
    std::tie(pos_, angle_) = *result;
    rotation_data_ = tetromino_.GetRotationData(angle_);
    matrix_->Insert(pos_, rotation_data_);
    last_move_ = Tetromino::Move::Rotation;
  }
}

void TetrominoSprite::SoftDrop() {
  if (State::OnFloor == state_) {
    return;
  }
  if (pos_.row() >= kMatrixFirstRow - kSkylineOffset) {
    events_.Push(Event::Type::DropScoreData, 1);
  }
  level_->Release();
}

void TetrominoSprite::HardDrop() {
  if (State::OnFloor == state_) {
    state_ = State::Commit;
    return;
  }
  auto drop_row = pos_.row();

  state_ = State::Commit;
  pos_ = matrix_->GetDropPosition(pos_, rotation_data_);
  level_->Release();

  last_move_ = Tetromino::Move::Down;
  events_.Push(Event::Type::DropScoreData, (pos_.row() - drop_row) * 2);
}

void TetrominoSprite::Left() {
  if (matrix_->IsValid(Position(pos_.row(), pos_.col() - 1), rotation_data_)) {
    pos_.dec_col();
    matrix_->Insert(pos_, rotation_data_);
    last_move_ = Tetromino::Move::Left;
    ResetDelayCounter();
  }
}

void TetrominoSprite::Right() {
  if (matrix_->IsValid(Position(pos_.row(), pos_.col() + 1), rotation_data_)) {
    pos_.inc_col();
    matrix_->Insert(pos_, rotation_data_);
    last_move_ = Tetromino::Move::Right;
    ResetDelayCounter();
  }
}

TetrominoSprite::State TetrominoSprite::Down(double delta_time) {
  switch (state_) {
    case State::Falling:
    case State::Generated:
      last_move_ = Tetromino::Move::Down;
      if (level_->WaitForMoveDown(delta_time)) {
        if (reset_delay_counter_ >= kResetsAllowed) {
          state_ = State::Commit;
        } else if (matrix_->IsValid(Position(pos_.row() + 1, pos_.col()), rotation_data_)) {
          pos_.inc_row();
          matrix_->Insert(pos_, rotation_data_);
          if (State::Generated == state_) {
            state_ = State::Falling;
            events_.Push(Event::Type::BattleNextTetrominoSuccessful);
          }
          if (!matrix_->IsValid(Position(pos_.row() + 1, pos_.col()), rotation_data_)) {
            events_.Push(Event::Type::OnFloor, Events::QueueRule::NoDuplicates);
            state_ = State::OnFloor;
          }
        } else {
          events_.Push(Event::Type::OnFloor, Events::QueueRule::NoDuplicates);
          state_ = State::OnFloor;
        }
      }
      break;
    case State::OnFloor:
      if (level_->WaitForLockDelay(delta_time)) {
        state_ = State::Commit;
        if (matrix_->IsAboveSkyline(pos_, rotation_data_)) {
          state_ = (got_lines_) ? State::KO : State::GameOver;
        }
      } else if (matrix_->IsValid(Position(pos_.row() + 1, pos_.col()), rotation_data_)) {
        events_.Push(Event::Type::ClearOnFloor, Events::QueueRule::NoDuplicates);
        state_ = State::Falling;
      }
      break;
    case State::Commit:
      {
        auto [lines_cleared, tspin_type, perfect_clear] = matrix_->Commit(tetromino_.type(), last_move_, pos_, rotation_data_);

        if (perfect_clear) {
          events_.Push(Event::Type::PerfectClear);
        }
        events_.Push(Event::Type::ClearedLinesScoreData, lines_cleared, pos_, tspin_type);
        state_ = State::Commited;
      }
      break;
    default:
      break;
  }
  return state_;
}
