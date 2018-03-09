#pragma once

#include "utility/text.h"
#include "game/tetromino.h"
#include "game/constants.h"

#include <memory>

enum Font { Normal15, Bold15, Normal20, Bold20, Normal25, Bold25, Normal30, Bold30, Normal35, Bold35, Normal45, Bold45, Normal55, Bold55, Normal100, Normal200 };

class Assets final {
 public:
  explicit Assets(SDL_Renderer *renderer);

  ~Assets() = default;

  Assets(const Assets&) = delete;

  std::shared_ptr<const Tetromino> GetTetromino(Tetromino::Type type) const { return tetrominos_.at(static_cast<int>(type) - 1); }

  const std::vector<std::shared_ptr<const Tetromino>>& GetTetrominos() const { return tetrominos_; }

  std::shared_ptr<SDL_Texture> GetAlphaTextures(Tetromino::Type type) const { return alpha_textures_.at(static_cast<int>(type) - 1); }

  virtual TTF_Font* GetFont(int id) const { return fonts_.at(id).get(); }

 private:
  using UniqueFontPtr = std::unique_ptr<TTF_Font, function_caller<void(TTF_Font*), &TTF_CloseFont>>;

  std::vector<UniqueFontPtr> fonts_;
  std::vector<std::shared_ptr<const Tetromino>> tetrominos_;
  std::vector<std::shared_ptr<SDL_Texture>> alpha_textures_;
};
