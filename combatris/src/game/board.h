#pragma once

#include "game/tetromino_generator.h"
#include "game/tetromino_sprite.h"
#include "game/matrix.h"

class Board final {
 public:
  Board();
  Board(const Board&) = delete;
  Board(const Board&&) = delete;
  ~Board() noexcept;

  void Up();

  void Down();

  void Left();

  void Right();

  operator SDL_Window*() const { return window_; }

  void Render(double delta_timer);

 private:
  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  SDL_Texture* border_texture_ = nullptr;
  Tetromino::Type current_tetromino_ = Tetromino::Type::I;
  std::unique_ptr<TetrominoSprite> tetromino_in_play_;
  std::shared_ptr<AssetManager> asset_manager_;
  std::unique_ptr<TetrominoGenerator> tetromino_generator_;
  std::shared_ptr<Matrix> matrix_;
};
