#include "game/tetromino.h"

void Tetromino::Render(int x, int y) const {
  SDL_Rect src_rc {0, BlockHeight, BlockWidth, BlockHeight };
  SDL_Rect dest_rc {x, y, BlockWidth, BlockHeight };

  SDL_RenderCopy(renderer_, tetromino_.get(), &src_rc, &dest_rc);
}

void Tetromino::Render(int, int, DisplayAngle) const {

}
