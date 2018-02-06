#pragma once

#include "game/assets.h"
#include "game/renderer.h"

class Pane : public RenderInterface {
 public:
  Pane(SDL_Renderer* renderer, int x, int y, const std::shared_ptr<Assets>& assets) : renderer_(renderer), x_(x), y_(y), assets_(assets) {}

 protected:
  void RenderText(int x, int y, Font font, const std::string& text, Color text_color) const {
    ::RenderText(renderer_, x, y, assets_->GetFont(font), text, text_color);
  }

  SDL_Renderer* renderer_;
  int x_;
  int y_;
  const std::shared_ptr<Assets>& assets_;
};
