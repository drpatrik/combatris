#pragma once

#include "game/asset_manager.h"

#include <memory>
#include <deque>

#include <SDL.h>

class Board final {
 public:
  Board();
  Board(const Board&) = delete;
  Board(const Board&&) = delete;
  ~Board() noexcept;

  operator SDL_Window*() const { return window_; }


  void Render(double delta_timer);

 private:
  SDL_Window *window_ = nullptr;
  SDL_Renderer *renderer_ = nullptr;
  std::shared_ptr<AssetManager> asset_manager_;
};
