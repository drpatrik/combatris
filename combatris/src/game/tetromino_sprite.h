#pragma once

#include "game/matrix.h"
#include "game/panes/level.h"

#include <optional>

namespace {

const Position kSpawnPosition = Position(0, 5);
const Tetromino::Angle kSpawnAngle = Tetromino::Angle::A0;

} // namespace

class TetrominoSprite {
 public:
  enum class State { Falling, OnFloor, Commit, Commited, GameOver, KO };
  enum class Rotation { Clockwise, CounterClockwise };

  TetrominoSprite(const Tetromino& tetromino, const std::shared_ptr<Level>& level, Events& events, const std::shared_ptr<Matrix>& matrix)
      : tetromino_(tetromino), level_(level), events_(events), matrix_(matrix) { MoveToStartPosition(); }

  void MoveToStartPosition() {
    pos_ = kSpawnPosition;
    rotation_data_ = tetromino_.GetRotationData(kSpawnAngle);
    if (!matrix_->IsValid(pos_, rotation_data_)) {
      state_ = (got_lines_) ? State::KO : State::GameOver;
      return;
    }
    matrix_->Insert(pos_, rotation_data_);
    level_->ResetTime();
    state_ = State::Falling;
    got_lines_ = false;
  }

  void Render(const std::shared_ptr<SDL_Texture>& texture) const {
    const Position adjusted_pos(pos_.row() - kVisibleRowStart, pos_.col() - kVisibleColStart);

    tetromino_.Render(adjusted_pos.x(), adjusted_pos.y(), texture.get(), angle_);
  }

  inline const Tetromino& tetromino() const { return tetromino_; }

  inline State state() const { return state_; }

  inline bool WaitForLockDelay() { return level_->WaitForLockDelay(); }

  void InsertLines(int lines) {
    got_lines_ = true;
    matrix_->InsertLines(lines);
    MoveToStartPosition();
  }

  void RemoveLines() {
    state_ = State::Falling;
    matrix_->RemoveLines();
  }

  void RotateClockwise();

  void RotateCounterClockwise();

  void SoftDrop();

  void HardDrop();

  void Left();

  void Right();

  State Down(double delta_time);

 protected:
  void ResetDelayCounter();

  std::optional<std::pair<Position, Tetromino::Angle>> TryRotation(Tetromino::Type type, const Position& current_pos, Tetromino::Angle current_angle, Rotation rotate);

 private:
  const Tetromino& tetromino_;
  std::shared_ptr<Level> level_;
  Events& events_;
  std::shared_ptr<Matrix> matrix_;
  TetrominoRotationData rotation_data_;
  Tetromino::Angle angle_ = kSpawnAngle;
  Position pos_ = kSpawnPosition;
  Tetromino::Move last_move_ = Tetromino::Move::None;
  int reset_delay_counter_ = 0;
  State state_ = State::GameOver;
  bool got_lines_ = false;
};
