#pragma once

#include "utility/text.h"

namespace utility {

class Texture {
 public:
  Texture() {}

  Texture(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, Color text_color) {
    std::tie(texture_, rc_.w, rc_.h) = CreateTextureFromText(renderer, font, text, text_color);
  }

  Texture(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, Color text_color, Color background_color) {
    std::tie(texture_, rc_.w, rc_.h) = CreateTextureFromFramedText(renderer, font, text, text_color, background_color);
  }

  Texture(std::tuple<std::shared_ptr<SDL_Texture>, int, int> texture) {
    texture_= UniqueTexturePtr{ std::get<0>(texture).get() };
    rc_.w = std::get<1>(texture);
    rc_.h = std::get<2>(texture);
  }

  inline bool is_null() const { return nullptr == texture_; }

  inline operator SDL_Texture*() { return texture_.get(); }

  inline operator const SDL_Rect&() { return rc_; }

  inline operator const SDL_Rect*() { return &rc_; }

  inline void SetX(int x) { rc_.x = x; }

  inline void SetY(int y) { rc_.y = y; }

  inline void SetXY(int x, int y) { rc_.x = x;  rc_.y = y; }

  inline int x() const { return rc_.x; }

  inline int center_x(int w) const { return utility::Center(w, rc_.w); }

  inline int center_y(int h) const { return utility::Center(h, rc_.h); }

  inline int y() const { return rc_.y; }

  inline int width() const { return rc_.w; }

  inline void SetWidth(int w) { rc_.w = w; }

  inline int height() const { return rc_.h; }

  inline void SetHeight(int h) { rc_.h = h; }

 private:
  UniqueTexturePtr texture_ = nullptr;
  SDL_Rect rc_ = {};
};

} // namespace texture
