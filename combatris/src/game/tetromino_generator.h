#pragma once

#include "game/assets.h"
#include "game/tetromino_sprite.h"

#include <deque>
#include <random>

class TetrominoGenerator final {
 public:
  TetrominoGenerator(std::shared_ptr<Matrix>& matrix, std::shared_ptr<Level>& level, Events& events, const std::shared_ptr<Assets>& assets)
      : matrix_(matrix), level_(level), events_(events), assets_(assets) {
    GenerateTetrominos();
  }

  std::shared_ptr<TetrominoSprite> Get(bool got_lines = false) {
    auto tetromino = tetrominos_queue_.front();

    tetrominos_queue_.pop_front();
    if (tetrominos_queue_.size() == kTetrominos.size()) {
      GenerateTetrominos();
    }
    return Get(tetromino, got_lines);
  }

  std::shared_ptr<TetrominoSprite> Get(Tetromino::Type type, bool got_lines = false) {
    return std::make_shared<TetrominoSprite>(*assets_->GetTetromino(type), level_,  events_, matrix_, got_lines);
  }

  void Reset() {
    tetrominos_queue_.clear();
    GenerateTetrominos();
  }

  void Put(const Tetromino& tetromino) { tetrominos_queue_.push_front(tetromino.type()); }

  void RenderFromQueue(size_t n, int x, int y) const { assets_->GetTetromino(tetrominos_queue_.at(n))->RenderTetromino(x, y); }

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
  std::shared_ptr<Level> level_;
  Events& events_;
  std::shared_ptr<Assets> assets_;
  std::deque<Tetromino::Type> tetrominos_queue_;
  mutable std::mt19937 engine_ { std::random_device{}() };
};
