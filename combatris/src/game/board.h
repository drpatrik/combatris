#pragma once

#include "game/tetromino_generator.h"
#include "game/tetromino_sprite.h"
#include "game/level.h"

class Board final {
 public:
  enum class Controls { RotateClockwise, RotateCounterClockwise, SoftDrop, HardDrop, Left, Right, HoldPiece };

  Board();
  Board(const Board&) = delete;
  Board(const Board&&) = delete;
  ~Board() noexcept;

  void GameControl(Controls control_pressed);

  void Render(double delta_timer);

  void NewGame();

  //operator SDL_Window*() const { return window_; }

 private:
  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  std::unique_ptr<TetrominoSprite> tetromino_in_play_;
  std::shared_ptr<AssetManager> asset_manager_;
  std::unique_ptr<TetrominoGenerator> tetromino_generator_;
  std::shared_ptr<Matrix> matrix_;
  std::shared_ptr<Level> level_;
};
