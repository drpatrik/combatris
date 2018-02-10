#pragma once

#include "game/coordinates.h"
#include "game/matrix.h"
#include "game/panes/level.h"

class TetrominoSprite {
 public:
  enum class Status { Continue, Commited };
  enum class Rotation { Clockwise, CounterClockwise };

  TetrominoSprite(const Tetromino& tetromino, const std::shared_ptr<Level>& level, Events& events, const std::shared_ptr<Matrix>& matrix)
      : tetromino_(tetromino), matrix_(matrix), level_(level), events_(events), rotation_data_(tetromino.GetRotationData(angle_)) {
    if (matrix_->IsValid(pos_, rotation_data_)) {
      matrix_->Insert(pos_, rotation_data_);
      level_->ResetTime();
      game_over_ = false;
    }
  }

  Tetromino::Type type() { return tetromino_.type(); }

  bool is_game_over() const { return game_over_; }

  void RotateClockwise();

  void RotateCounterClockwise();

  void SoftDrop();

  void HardDrop();

  void Left();

  void Right();

  Status Down(double delta_time);

 protected:
  const Position kSpawnPosition = Position(0, 5);

  std::tuple<bool, Position, Tetromino::Angle> TryRotation(Tetromino::Type type, const Position& current_pos, Tetromino::Angle current_angle, Rotation rotate);

 private:
  const Tetromino& tetromino_;
  Tetromino::Angle angle_ = Tetromino::Angle::A0;
  Position pos_ = kSpawnPosition;
  std::shared_ptr<Matrix> matrix_;
  std::shared_ptr<Level> level_;
  Events& events_;
  TetrominoRotationData rotation_data_;
  bool floor_reached_ = false;
  Tetromino::Moves last_move_ = Tetromino::Moves::None;
  bool game_over_ = true;
};
