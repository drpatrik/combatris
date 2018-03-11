#pragma once

#include "utility/fonts.h"
#include "game/predefined_fonts.h"
#include "utility/function_caller.h"
#include "game/tetromino.h"

#include <memory>

class Assets final {
 public:
  explicit Assets(SDL_Renderer *renderer);

  ~Assets() = default;

  Assets(const Assets&) = delete;

  TTF_Font* GetFont(const Font& font) const { return fonts_.Get(font); }

  TTF_Font* GetFont(Font::Typeface typeface, Font::Emphasis emphasis, int size) const { return fonts_.Get(typeface, emphasis, size); }

  std::shared_ptr<const Tetromino> GetTetromino(Tetromino::Type type) const { return tetrominos_.at(static_cast<int>(type) - 1); }

  const std::vector<std::shared_ptr<const Tetromino>>& GetTetrominos() const { return tetrominos_; }

  std::shared_ptr<SDL_Texture> GetAlphaTextures(Tetromino::Type type) const { return alpha_textures_.at(static_cast<int>(type) - 1); }

 private:
   using UniqueFontPtr = std::unique_ptr<TTF_Font, function_caller<void(TTF_Font*), &TTF_CloseFont>>;

  std::vector<std::shared_ptr<const Tetromino>> tetrominos_;
  std::vector<std::shared_ptr<SDL_Texture>> alpha_textures_;
  Fonts fonts_;
};
