#pragma once

#include "utility/text.h"
#include "game/tetromino.h"
#include "game/constants.h"

#include <memory>

class Assets final {
 public:
  explicit Assets(SDL_Renderer *renderer);

  ~Assets() = default;

  Assets(const Assets&) = delete;

  std::shared_ptr<const Tetromino> GetTetromino(Tetromino::Type type) const { return tetrominos_.at(static_cast<int>(type) - 1); }

  const std::vector<std::shared_ptr<const Tetromino>>& GetTetrominos() const { return tetrominos_; }

  virtual TTF_Font* GetFont(int id) const { return fonts_.at(id).get(); }

 private:
  using UniqueFontPtr = std::unique_ptr<TTF_Font, function_caller<void(TTF_Font*), &TTF_CloseFont>>;

  std::vector<UniqueFontPtr> fonts_;
  std::vector<std::shared_ptr<const Tetromino>> tetrominos_;
};
