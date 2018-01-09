#pragma once

#include "game/constants.h"
#include "game/asset_manager.h"
#include "game/tetromino_sprite.h"

#include <random>

class TetrominoGenerator {
 public:
  TetrominoGenerator(SDL_Renderer *renderer, AssetManager& asset_manager);

  std::unique_ptr<TetrominoSprite> Get();
  std::unique_ptr<TetrominoSprite> Get(Tetromino::Type type);

 private:
  SDL_Renderer* renderer_;
  AssetManager& asset_manager_;
  std::vector<std::shared_ptr<Tetromino>> tetrominos_;
  mutable std::mt19937 engine_ {std::random_device{}()};
  mutable std::uniform_int_distribution<int> distribution_{ 1, kNumTetrominos };
};
