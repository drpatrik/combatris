#pragma once

#include "game/matrix.h"
#include "game/panes/level.h"

#if __has_include(<optional>)
#include <optional>
namespace opt = std;
#else
#include <experimental/optional>
namespace opt = std::experimental;
#endif

namespace {

const Position kSpawnPosition = Position(0, 5);
const Tetromino::Angle kSpawnAngle = Tetromino::Angle::A0;

} // namespace

class TetrominoSprite {
 public:
  enum class State { Falling, OnFloor, Commit, Commited, GameOver, KO };
  enum class Rotation { Clockwise, CounterClockwise };

  TetrominoSprite(const Tetromino& tetromino, const std::shared_ptr<Level>& level, Events& events,
                  const std::shared_ptr<Matrix>& matrix, bool got_lines = false)
      : tetromino_(tetromino), level_(level), events_(events), matrix_(matrix) {
    pos_ = kSpawnPosition;
    rotation_data_ = tetromino_.GetRotationData(kSpawnAngle);
    if (!matrix_->IsValid(pos_, rotation_data_)) {
      if (got_lines) {
        std::cout << "KO" << std::endl;
      }
      state_ = (got_lines) ? State::KO : State::GameOver;
      return;
    }
    matrix_->Insert(pos_, rotation_data_);
    level_->ResetTime();
    state_ = State::Falling;
  }

  void Render(const std::shared_ptr<SDL_Texture>& texture) const {
    const Position adjusted_pos(pos_.row() - kVisibleRowStart, pos_.col() - kVisibleColStart);

    tetromino_.Render(adjusted_pos.x(), adjusted_pos.y(), texture.get(), angle_);
  }

  inline const Tetromino& tetromino() const { return tetromino_; }

  inline State state() const { return state_; }

  inline bool WaitForLockDelay() { return level_->WaitForLockDelay(); }

  void RotateClockwise();

  void RotateCounterClockwise();

  void SoftDrop();

  void HardDrop();

  void Left();

  void Right();

  State Down(double delta_time);

 protected:
  void ResetDelayCounter();

  opt::optional<std::pair<Position, Tetromino::Angle>> TryRotation(Tetromino::Type type, const Position& current_pos, Tetromino::Angle current_angle, Rotation rotate);

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
};
