#include "game/play_field.h"
#include "game/constants.h"

#include <iostream>

const SDL_Rect kClipRect { 0, kBoardStartY, kWidth, kHeight }; // We only care about Y position

PlayField::PlayField() {
  window_ = SDL_CreateWindow("Combatris", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, kWidth, kHeight, SDL_WINDOW_RESIZABLE);
  if (nullptr == window_) {
    std::cout << "Failed to create window : " << SDL_GetError() << std::endl;
    exit(-1);
  }
  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
  if (nullptr == renderer_) {
    std::cout << "Failed to create renderer : " << SDL_GetError() << std::endl;
    exit(-1);
  }
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
  SDL_RenderSetLogicalSize(renderer_, kWidth, kHeight);
}

PlayField::~PlayField() noexcept {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

void PlayField::Render(double /*delta_time*/) {
  SDL_RenderClear(renderer_);
  SDL_RenderSetClipRect(renderer_, &kClipRect);

  SDL_RenderPresent(renderer_);
}
