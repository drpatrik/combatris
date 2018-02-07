#pragma once

#include "game/constants.h"

#include <SDL.h>

class RenderInterface {
 public:
  virtual void Render() const = 0;
};

inline void RenderBlock(SDL_Renderer* renderer, int x, int y, SDL_Texture* texture) {
  SDL_Rect dest_rc { x, y, kBlockWidth, kBlockHeight };

  SDL_RenderCopy(renderer, texture, nullptr, &dest_rc);
}

inline void RenderGhost(SDL_Renderer* renderer, int x, int y, const SDL_Color& color) {
  SDL_Rect rc { x, y, kBlockWidth, kBlockHeight };

  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(renderer, &rc);

  rc = { x + 2, y + 2, kBlockWidth - 4, kBlockHeight - 4 };

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderFillRect(renderer, &rc);
}
