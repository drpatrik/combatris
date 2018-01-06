#include "tools/timer.h"
#include "game/coordinates.h"
#include "game/board.h"

#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <thread>
#include <sstream>
#include <iostream>

class Combatris {
 public:
  using HighResClock = std::chrono::high_resolution_clock;

  Combatris() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
      std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
      exit(-1);
    }
    if (TTF_Init() != 0) {
      std::cout << "TTF_Init Error: " << TTF_GetError() << std::endl;
      exit(-1);
    }
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) != 0) {
      std::cout << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
      exit(-1);
    }
  }

  ~Combatris() {
    SDL_Quit();
    Mix_CloseAudio();
    Mix_Quit();
  }

  static void Play() {
    bool quit = false;
    DeltaTimer delta_timer;
    Board board;

    while (!quit) {
      SDL_Event event;

      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          quit = true;
          break;
        }
        switch (event.type) {
          case SDL_SCANCODE_LEFT:
            if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
              delta_timer.Reset();
            }

          case SDL_KEYDOWN:
            if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
              delta_timer.Reset();
            } if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
              board.Up();
            }
            if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
              board.Left();
            } else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
              board.Right();
            }
            break;
#if !defined(NDEBUG)
          case SDL_MOUSEMOTION: {

            int mouseX = event.motion.x;
            int mouseY = event.motion.y;
            int row = pixel_to_row(mouseY);
            int col = pixel_to_col(mouseX);

            std::stringstream ss;
            ss << "X: " << mouseX << " Y: " << mouseY << " Row: " <<  row << " Col: " << col;

            SDL_SetWindowTitle(board, ss.str().c_str());
          } break;
#endif
          case SDL_MOUSEBUTTONDOWN:
            switch (event.button.button) {
              case SDL_BUTTON_LEFT:
                break;
            }
        }
      }
      board.Render(delta_timer.GetDelta());
    }
  }
};

int main(int, char * []) {
  Combatris combatris;

  combatris.Play();

  return 0;
}
