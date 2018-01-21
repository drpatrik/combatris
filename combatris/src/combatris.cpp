#include "tools/timer.h"
#include "game/board.h"

#include <SDL_mixer.h>

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
          case SDL_KEYDOWN:
            if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
              board.GameControl(Board::Controls::HardDrop);
            } else if (event.key.keysym.scancode == SDL_SCANCODE_A) {
              board.GameControl(Board::Controls::RotateCounterClockwise);
            } else if (event.key.keysym.scancode == SDL_SCANCODE_UP || event.key.keysym.scancode == SDL_SCANCODE_S) {
              board.GameControl(Board::Controls::RotateClockwise);
            } else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
              board.GameControl(Board::Controls::Left);
            } else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
              board.GameControl(Board::Controls::Right);
            } else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
              board.GameControl(Board::Controls::SoftDrop);
            }
            break;
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
