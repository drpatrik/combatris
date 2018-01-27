#include "utility/timer.h"
#include "game/board.h"

#include <SDL_mixer.h>

class Combatris {
 public:
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
    TTF_Quit();
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
            if (event.key.keysym.scancode == SDL_SCANCODE_N) {
              board.NewGame();
            } else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
              board.GameControl(Board::Controls::HardDrop);
            } else if (event.key.keysym.scancode == SDL_SCANCODE_Z) {
              board.GameControl(Board::Controls::RotateCounterClockwise);
            } else if (event.key.keysym.scancode == SDL_SCANCODE_UP || event.key.keysym.scancode == SDL_SCANCODE_X) {
              board.GameControl(Board::Controls::RotateClockwise);
            } else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
              board.GameControl(Board::Controls::Left);
            } else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
              board.GameControl(Board::Controls::Right);
            } else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
              board.GameControl(Board::Controls::SoftDrop);
            } else if (event.key.keysym.scancode == SDL_SCANCODE_LSHIFT || event.key.keysym.scancode == SDL_SCANCODE_C) {
              board.GameControl(Board::Controls::HoldPiece);
            } else if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE || event.key.keysym.scancode == SDL_SCANCODE_F1 || event.key.keysym.scancode == SDL_SCANCODE_P) {
              board.GameControl(Board::Controls::Pause);
            }
            break;
          case SDL_MOUSEBUTTONDOWN:
            switch (event.button.button) {
              case SDL_BUTTON_LEFT:
                break;
            }
        }
      }
      board.Update(delta_timer.GetDelta());
    }
  }
};

int main(int, char * []) {
  Combatris combatris;

  combatris.Play();

  return 0;
}
