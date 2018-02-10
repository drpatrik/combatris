#include "utility/timer.h"
#include "game/board.h"

#include <SDL_mixer.h>

namespace {

uint32_t kFirstKeyRepeatTime = 150; // milliseconds
uint32_t kKeyRepeatTime = 50; // milliseconds

}

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
    if (SDL_NumJoysticks() > 0) {
      SDL_JoystickEventState(SDL_ENABLE);
      SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
      joystick_ = SDL_JoystickOpen(0);
      if (nullptr == joystick_) {
        std::cout << "Warning: Unable to open game controller! SDL Error: " << SDL_GetError() << std::endl;
        exit(-1);
      }
      std::cout << "Joystick found: " << SDL_JoystickName(joystick_) << std::endl;
    }
  }

  ~Combatris() {
    if (joystick_ != nullptr) {
      SDL_JoystickClose(joystick_);
    }
    SDL_Quit();
    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();
  }

  void Play() {
    bool quit = false;
    Board board;
    DeltaTimer delta_timer;
    bool button_pressed = false;
    uint32_t repeat_counter = 0;
    uint32_t repeat_threshold = kFirstKeyRepeatTime;
    std::function<void()> function_to_repeat;

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
              board.Pause();
            }
            break;
          case SDL_JOYBUTTONDOWN:
            if (!button_pressed) {
              repeat_counter = SDL_GetTicks();
              button_pressed = true;
            }
            if (event.jbutton.button == 6) { // pad down
              function_to_repeat = [&board]() { board.GameControl(Board::Controls::SoftDrop); };
              function_to_repeat();
            }  else if (event.jbutton.button == 7) { // pad left
              function_to_repeat = [&board]() { board.GameControl(Board::Controls::Left); };
              function_to_repeat();
            }  else if (event.jbutton.button == 5) { // pad right
              function_to_repeat = [&board]() { board.GameControl(Board::Controls::Right); };
              function_to_repeat();
            } else if (event.jbutton.button == 15) { // Square
              board.GameControl(Board::Controls::RotateCounterClockwise);
            }  else if (event.jbutton.button == 13) { // Circle
              board.GameControl(Board::Controls::RotateClockwise);
            }  else if (event.jbutton.button == 12) { // Triangle
              board.GameControl(Board::Controls::HoldPiece);
            }  else if (event.jbutton.button == 14) { // X
              board.GameControl(Board::Controls::HardDrop);
            }  else if (event.jbutton.button == 3) { // Start
              board.NewGame();
            } else if (event.jbutton.button == 0) { // Select
              board.Pause();
            }
            break;
          case SDL_JOYBUTTONUP:
            button_pressed = false;
            function_to_repeat = nullptr;
            repeat_threshold = kFirstKeyRepeatTime;
            break;
        }
      }
      if (button_pressed && (SDL_GetTicks() - repeat_counter) > repeat_threshold) {
        if (function_to_repeat) {
          function_to_repeat();
        }
        repeat_counter = SDL_GetTicks();
        repeat_threshold = kKeyRepeatTime;
      }
      board.Update(delta_timer.GetDelta());
    }
  }

 private:
  SDL_Joystick* joystick_ = nullptr;
};

int main(int, char * []) {
  Combatris combatris;

  combatris.Play();

  return 0;
}
