#pragma once

#include "utility/color.h"

#include <SDL.h>
#include <string>

class Pane {
 public:
  enum class HeaderOrientation { Top, Left, Right };

  Pane(SDL_Renderer* renderer, int x, int y, int w, int h)
      : renderer_(renderer), x_(x), y_(y), w_(w), h_(h), title_() {}
  Pane(SDL_Renderer* renderer, int x, int y, int w, int h, const std::string& title)
      : renderer_(renderer), x_(x), y_(y),  w_(w), h_(h), title_(title) {}

  void Render();

  virtual void RenderPane() = 0;

 protected:
  SDL_Renderer* renderer_;
  int x_;
  int y_;
  int w_;
  int h_;
  std::string title_;
  /*Color frame_color_ = Color::Black;;
  Color text_color_;
  Color header_color_;
  Color body_color_;
  bool has_frame_ = false;
  HeaderOrientation orientation_ = HeaderOrientation::Top;*/
};
