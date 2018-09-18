#pragma once

#include "utility/fonts.h"
#include "game/predefined_fonts.h"
#include "utility/function_caller.h"
#include "game/tetromino.h"

class Assets final {
 public:
  enum class Type { Checkmark, Circle };

  explicit Assets(SDL_Renderer *renderer);

  ~Assets() noexcept = default;

  Assets(const Assets&) = delete;

  TTF_Font* GetFont(const utility::Font& font) const { return fonts_->Get(font); }

  TTF_Font* GetFont(utility::Font::Typeface typeface, utility::Font::Emphasis emphasis, int size) const {
    return fonts_->Get(typeface, emphasis, size);
  }

  std::shared_ptr<utility::Fonts> fonts() { return fonts_; }

  std::tuple<std::shared_ptr<SDL_Texture>, int, int> GetTexture(Type type) const;

  std::shared_ptr<const Tetromino> GetTetromino(Tetromino::Type type) const { return tetrominos_.at(static_cast<int>(type) - 1); }

  const std::vector<std::shared_ptr<const Tetromino>>& GetTetrominos() const { return tetrominos_; }

  std::shared_ptr<SDL_Texture> GetAlphaTextures(Tetromino::Type type) const { return alpha_textures_.at(static_cast<int>(type) - 1); }

  std::vector<std::shared_ptr<SDL_Texture>> GetHourGlassTextures() const { return hourglass_textures_; }

 private:
  using UniqueFontPtr = std::unique_ptr<TTF_Font, utility::function_caller<void(TTF_Font*), &TTF_CloseFont>>;

  std::vector<std::shared_ptr<const Tetromino>> tetrominos_;
  std::vector<std::shared_ptr<SDL_Texture>> textures_;
  std::vector<std::shared_ptr<SDL_Texture>> alpha_textures_;
  std::vector<std::shared_ptr<SDL_Texture>> hourglass_textures_;
  std::shared_ptr<utility::Fonts> fonts_;
};
