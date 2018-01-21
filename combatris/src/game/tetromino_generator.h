#pragma once

#include "game/constants.h"
#include "game/asset_manager.h"
#include "game/tetromino_sprite.h"

#include <random>

class TetrominoGenerator {
 public:
  TetrominoGenerator(std::shared_ptr<Matrix>& matrix, Level& level, const std::shared_ptr<AssetManager>& asset_manager)
      : matrix_(matrix), level_(level), asset_manager_(asset_manager) {}

  std::unique_ptr<TetrominoSprite> Get() {  return Get(static_cast<Tetromino::Type>(distribution_(engine_))); }

  std::unique_ptr<TetrominoSprite> Get(Tetromino::Type type) { return std::make_unique<TetrominoSprite>(*asset_manager_->GetTetromino(type), level_,  matrix_); }

 private:
  std::shared_ptr<Matrix> matrix_;
  Level& level_;
  std::shared_ptr<AssetManager> asset_manager_;
  mutable std::mt19937 engine_ {std::random_device{}()};
  mutable std::uniform_int_distribution<int> distribution_{ 1, kNumTetrominos };
};
