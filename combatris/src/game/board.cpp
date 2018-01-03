#include "game/board.h"

#include <iostream>

Board::Board() {
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

  asset_manager_ = std::make_shared<AssetManager>(renderer_);
}

Board::~Board() noexcept {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

void Board::Render(double /*delta_time*/) {
  SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0);
  SDL_RenderClear(renderer_);

  auto sprite = asset_manager_->GetSprite(SpriteID::O_Block);

  SDL_Rect rc { 0, 0,  sprite->width(), sprite->height() };

  SDL_RenderCopy(renderer_, (*sprite)(), nullptr, &rc);

  sprite = asset_manager_->GetSprite(SpriteID::I_Block);

  rc =  { BlockWidth * 2, BlockHeight * 2,  sprite->width(), sprite->height() };

  SDL_RenderCopy(renderer_, (*sprite)(), nullptr, &rc);

  rc =  { BlockWidth * 6, BlockHeight * 3,  sprite->width(), sprite->height() };

  SDL_RenderCopy(renderer_, (*sprite)(), nullptr, &rc);

  SDL_RenderPresent(renderer_);
}
