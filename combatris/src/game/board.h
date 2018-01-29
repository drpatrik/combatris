#pragma once

#include "game/tetromino_generator.h"
#include "game/scoring.h"

class Board final {
 public:
  enum class Controls { RotateClockwise, RotateCounterClockwise, SoftDrop, HardDrop, Left, Right, HoldPiece, Pause };

  Board();

  Board(const Board&) = delete;

  Board(const Board&&) = delete;

  ~Board() noexcept;

  void NewGame();

  void GameControl(Controls control_pressed);

  void Update(double delta_timer);

 protected:
  void Render(double delta_timer);

 private:
  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  std::unique_ptr<TetrominoSprite> tetromino_in_play_;
  std::shared_ptr<Assets> assets_;
  std::unique_ptr<TetrominoGenerator> tetromino_generator_;
  std::shared_ptr<Matrix> matrix_;
  std::shared_ptr<Level> level_;
  std::shared_ptr<Scoring> scoring_;
  Events events_;
  std::deque<Tetromino::Type> next_;
};
