#pragma once

#include "game/constants.h"
#include "game/assets.h"
#include "game/tetromino_sprite.h"

#include <deque>
#include <random>

class TetrominoGenerator {
 public:
  TetrominoGenerator(std::shared_ptr<Matrix>& matrix, Level& level, const std::shared_ptr<Assets>& assets)
      : matrix_(matrix), level_(level), assets_(assets) {
    GenerateTetrominos();
  }

  void PushFront(Tetromino::Type type) { tetrominos_queue_.push_front(type); }

  std::unique_ptr<TetrominoSprite> Get() {
    auto tetromino = tetrominos_queue_.front();

    tetrominos_queue_.pop_front();
    if (tetrominos_queue_.size() == kTetrominos.size()) {
      GenerateTetrominos();
    }
    return Get(tetromino);
  }

  std::unique_ptr<TetrominoSprite> Get(Tetromino::Type type) {
    return std::make_unique<TetrominoSprite>(*assets_->GetTetromino(type), level_,  matrix_);
  }

  void NewGame() { tetrominos_queue_.clear(); GenerateTetrominos(); }

  void RenderFromQueue(size_t n, int x, int y) const { assets_->GetTetromino(tetrominos_queue_.at(n))->Render(x, y); }

 protected:
  const std::deque<Tetromino::Type> kTetrominos = { Tetromino::Type::I, Tetromino::Type::J, Tetromino::Type::L, Tetromino::Type::O, Tetromino::Type::S, Tetromino::Type::T, Tetromino::Type::Z };

  void GenerateTetrominos() {
    auto tetrominos = kTetrominos;
    std::shuffle(std::begin(tetrominos), std::end(tetrominos), engine_);
    if (!tetrominos_queue_.empty()) {
      tetrominos_queue_.insert(tetrominos_queue_.end(), tetrominos.begin(), tetrominos.end());
    } else {
      tetrominos_queue_ = tetrominos;
      tetrominos = kTetrominos;
      std::shuffle(std::begin(tetrominos), std::end(tetrominos), engine_);
      tetrominos_queue_.insert(tetrominos_queue_.end(), tetrominos.begin(), tetrominos.end());
    }
  }

 private:
  std::shared_ptr<Matrix> matrix_;
  Level& level_;
  std::shared_ptr<Assets> assets_;
  std::deque<Tetromino::Type> tetrominos_queue_;
  mutable std::mt19937 engine_ {std::random_device{}()};
  mutable std::uniform_int_distribution<int> distribution_{ 0, static_cast<int>(kTetrominos.size()) - 1 };
};
