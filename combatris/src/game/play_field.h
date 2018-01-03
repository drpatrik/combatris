#pragma once

#include <memory>
#include <deque>

#include <SDL.h>

class PlayField final {
 public:
  PlayField();
  PlayField(const PlayField&) = delete;
  PlayField(const PlayField&&) = delete;
  ~PlayField() noexcept;

  operator SDL_Window*() const { return window_; }


  void Render(double delta_timer);

 private:
  SDL_Window *window_ = nullptr;
  SDL_Renderer *renderer_ = nullptr;
};
