#pragma once

#include "utility/fonts.h"
#include "game/predefined_fonts.h"
#include "utility/function_caller.h"
#include "game/tetromino.h"

class Assets final {
 public:
  enum class Type { Checkmark };

  explicit Assets(SDL_Renderer *renderer);

  ~Assets() = default;

  Assets(const Assets&) = delete;

  TTF_Font* GetFont(const Font& font) const { return fonts_.Get(font); }

  TTF_Font* GetFont(Font::Typeface typeface, Font::Emphasis emphasis, int size) const { return fonts_.Get(typeface, emphasis, size); }

  std::tuple<std::shared_ptr<SDL_Texture>, int, int> GetTexture(Type type) const;

  std::shared_ptr<const Tetromino> GetTetromino(Tetromino::Type type) const { return tetrominos_.at(static_cast<int>(type) - 1); }

  const std::vector<std::shared_ptr<const Tetromino>>& GetTetrominos() const { return tetrominos_; }

  std::shared_ptr<SDL_Texture> GetAlphaTextures(Tetromino::Type type) const { return alpha_textures_.at(static_cast<int>(type) - 1); }

  std::vector<std::shared_ptr<SDL_Texture>> GetHourGlassTextures() const { return hourglass_textures_; }

 private:
   using UniqueFontPtr = std::unique_ptr<TTF_Font, function_caller<void(TTF_Font*), &TTF_CloseFont>>;

  std::vector<std::shared_ptr<const Tetromino>> tetrominos_;
  std::vector<std::shared_ptr<SDL_Texture>> textures_;
  std::vector<std::shared_ptr<SDL_Texture>> alpha_textures_;
  std::vector<std::shared_ptr<SDL_Texture>> hourglass_textures_;
  Fonts fonts_;
};
