#pragma once

#include "game/constants.h"
#include "game/asset_manager.h"
#include "game/tetromino_sprite.h"

#include <random>

class TetrominoGenerator {
 public:
  TetrominoGenerator(SDL_Renderer *renderer, const std::shared_ptr<AssetManager>& asset_manager);

  std::unique_ptr<TetrominoSprite> Get();
  std::unique_ptr<TetrominoSprite> Get(Tetromino::Type type);

  std::vector<std::shared_ptr<const Tetromino>> GetTetrominos() const { return tetrominos_; }

 private:
  SDL_Renderer* renderer_;
  std::shared_ptr<AssetManager> asset_manager_;
  std::vector<std::shared_ptr<const Tetromino>> tetrominos_;
  mutable std::mt19937 engine_ {std::random_device{}()};
  mutable std::uniform_int_distribution<int> distribution_{ 1, kNumTetrominos };
};
