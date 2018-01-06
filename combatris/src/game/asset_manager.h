#pragma once

#include "game/tetromino.h"
#include "game/constants.h"
#include "tools/function_caller.h"

#include <memory>
#include <exception>
#include <random>

#include <SDL_ttf.h>

enum Font { Normal, Bold, Small, Large };

class AssetManager final {
 public:
  explicit AssetManager(SDL_Renderer *renderer);

  AssetManager(const AssetManager&) = delete;

  virtual ~AssetManager() noexcept {};

  virtual std::shared_ptr<const Tetromino> GetTetromino() const { return tetrominos_.at(distribution_(engine_)); }

  virtual std::shared_ptr<const Tetromino> GetTetromino(Tetromino::Type type) {
    if (type == Tetromino::Type::Invalid) {
      throw std::invalid_argument("Type not allowed");
    }
    return tetrominos_.at(static_cast<int>(type) - 1); }

  virtual TTF_Font *GetFont(int id) const { return fonts_[id].get(); }

 private:
  using UniqueFontPtr = std::unique_ptr<TTF_Font, function_caller<void(TTF_Font*), &TTF_CloseFont>>;

  std::vector<UniqueFontPtr> fonts_;
  std::vector<std::shared_ptr<Tetromino>> tetrominos_;
  mutable std::mt19937 engine_ {std::random_device{}()};
  mutable std::uniform_int_distribution<int> distribution_{ 0, kNumTetrominos - 1 };
};
