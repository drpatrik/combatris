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

  operator SDL_Texture*() { return texture_.get(); }

  operator const SDL_Rect&() { return rc_; }

  operator const SDL_Rect*() { return &rc_; }

  void SetX(int x) { rc_.x = x; }

  void SetY(int y) { rc_.y = y; }

  void SetXY(int x, int y) { rc_.x = x;  rc_.y = y; }

  int x() const { return rc_.x; }

  int y() const { return rc_.y; }

  int width() const { return rc_.w; }

  int height() const { return rc_.h; }

 private:
  UniqueTexturePtr texture_ = nullptr;
  SDL_Rect rc_ = {};
};

} // namespace texture
