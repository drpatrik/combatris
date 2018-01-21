#pragma once

#include "game/constants.h"
#include "game/asset_manager.h"
#include "game/tetromino_sprite.h"

#include <random>

class TetrominoGenerator {
 public:
  TetrominoGenerator(std::shared_ptr<Matrix>& matrix, Level& level, const std::shared_ptr<AssetManager>& asset_manager)
      : matrix_(matrix), level_(level), asset_manager_(asset_manager) {
    GenerateTetrominos();
  }

  std::unique_ptr<TetrominoSprite> Get() {
    auto tetromino = tetrominos_.back();

    tetrominos_.pop_back();
    if (tetrominos_.empty()) {
      GenerateTetrominos();
    }
    return Get(tetromino);
  }

  std::unique_ptr<TetrominoSprite> Get(Tetromino::Type type) {
    return std::make_unique<TetrominoSprite>(*asset_manager_->GetTetromino(type), level_,  matrix_);
  }

  void Reset() {
    GenerateTetrominos();
    level_.Reset();
  }

  std::shared_ptr<const Tetromino> next() { return asset_manager_->GetTetromino(tetrominos_.back()); }

 protected:

  void GenerateTetrominos() {
    const std::vector<Tetromino::Type> kTetrominos = { Tetromino::Type::I, Tetromino::Type::J, Tetromino::Type::L, Tetromino::Type::O, Tetromino::Type::S, Tetromino::Type::T, Tetromino::Type::Z };

    tetrominos_ = kTetrominos;
    std::shuffle(std::begin(tetrominos_), std::end(tetrominos_), engine_);
  }

 private:
  std::shared_ptr<Matrix> matrix_;
  Level& level_;
  std::shared_ptr<AssetManager> asset_manager_;
  std::vector<Tetromino::Type>  tetrominos_;
  mutable std::mt19937 engine_ {std::random_device{}()};
  mutable std::uniform_int_distribution<int> distribution_{ 0, kNumTetrominos - 1 };
};
