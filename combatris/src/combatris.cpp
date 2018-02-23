#include "utility/timer.h"
#include "game/tetrion.h"

#include <SDL_mixer.h>
#include <set>

namespace {

// DAS settings
uint32_t kRepeatDelay = 150; // milliseconds
uint32_t kRepeatInterval = 85; // milliseconds
// PS3 Controller Mappings
int kJoystick_SoftDrop = 6; // Pad Down
int kJoystick_Left = 7; // Pad Left
int kJoystick_Right = 5; // Pad Right
int kJoystick_RotateCounterClockwise = 15; // Square button
int kJoystick_RotateClockwise = 13; // Circle button
int kJoystick_HoldQueue = 12; // Triangle Button
int kJoystick_HardDrop = 14; // X button
int kJoystick_Start = 3; // Start button
int kJoystick_Pause = 0; // Select button

const std::set<Tetrion::Controls> kAutoRepeatControls = { Tetrion::Controls::SoftDrop, Tetrion::Controls::Left, Tetrion::Controls::Right };

} // namespace

class Combatris {
 public:
  using RepeatFunc = std::function<void()>;

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
    SDL_JoystickEventState(SDL_ENABLE);
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    if (SDL_NumJoysticks() > 0) {
      AttachJoystick(0);
    }
  }

  ~Combatris() {
    tetrion_.reset();
    DetachJoystick(joystick_index_);
    SDL_Quit();
    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();
  }

  void Repeat(const bool button_pressed, const Tetrion::Controls control, Tetrion::Controls& previous_control, RepeatFunc& func, int64_t& counter) {
    if (button_pressed && previous_control == control) {
      return;
    }
    previous_control = control;
    func = [this, control]() { tetrion_->GameControl(control); };
    func();
    counter = time_in_ms();
  }

  void AttachJoystick(int index) {
    if (nullptr != joystick_) {
      return;
    }
    joystick_ = SDL_JoystickOpen(index);
    if (nullptr == joystick_) {
      std::cout << "Warning: Unable to open game controller! SDL Error: " << SDL_GetError() << std::endl;
      exit(-1);
    }
    joystick_index_ = index;
    std::cout << "Joystick found: " << SDL_JoystickName(joystick_) << std::endl;
  }

  void DetachJoystick(int index) {
    if (nullptr == joystick_ || index != joystick_index_) {
      return;
    }
    SDL_JoystickClose(joystick_);
    joystick_ = nullptr;
    joystick_index_ = -1;
  }

  Tetrion::Controls TranslateKeyboardCommands(const SDL_Event& event, bool button_pressed) {
    auto current_control = Tetrion::Controls::None;

    if (event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
      current_control = Tetrion::Controls::SoftDrop;
    } else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
      current_control = Tetrion::Controls::Left;
    } else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
      current_control = Tetrion::Controls::Right;
    } else if (!button_pressed && event.key.keysym.scancode == SDL_SCANCODE_Z) {
      current_control = Tetrion::Controls::RotateCounterClockwise;
    } else if (!button_pressed && (event.key.keysym.scancode == SDL_SCANCODE_UP || event.key.keysym.scancode == SDL_SCANCODE_X)) {
      current_control = Tetrion::Controls::RotateClockwise;
    } else if (event.key.keysym.scancode == SDL_SCANCODE_LSHIFT || event.key.keysym.scancode == SDL_SCANCODE_C) {
      current_control = Tetrion::Controls::Hold;
    } else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
      current_control = Tetrion::Controls::HardDrop;
    } else if (event.key.keysym.scancode == SDL_SCANCODE_N) {
      current_control = Tetrion::Controls::Start;
    } else if (event.key.keysym.scancode == SDL_SCANCODE_P) {
      current_control = Tetrion::Controls::Pause;
    }
    return current_control;
  }

  Tetrion::Controls TranslateJoystickCommands(const SDL_Event& event) {
    auto current_control = Tetrion::Controls::None;

    if (event.jbutton.button == kJoystick_SoftDrop) {
      current_control = Tetrion::Controls::SoftDrop;
    } else if (event.jbutton.button == kJoystick_Left) {
      current_control = Tetrion::Controls::Left;
    } else if (event.jbutton.button == kJoystick_Right) {
      current_control = Tetrion::Controls::Right;
    } else if (event.jbutton.button == kJoystick_RotateCounterClockwise) {
      current_control = Tetrion::Controls::RotateCounterClockwise;
    }  else if (event.jbutton.button == kJoystick_RotateClockwise) {
      current_control = Tetrion::Controls::RotateClockwise;
    }  else if (event.jbutton.button == kJoystick_HoldQueue) {
      current_control = Tetrion::Controls::Hold;
    }  else if (event.jbutton.button == kJoystick_HardDrop) {
      current_control = Tetrion::Controls::HardDrop;
    }  else if (event.jbutton.button == kJoystick_Start) {
      current_control = Tetrion::Controls::Start;
    } else if (event.jbutton.button == kJoystick_Pause) {
      current_control = Tetrion::Controls::Pause;
    }
    return current_control;
  }

  void Play() {
    bool quit = false;
    DeltaTimer delta_timer;
    bool button_pressed = false;
    int64_t repeat_counter = 0;
    int64_t repeat_threshold = kRepeatDelay;
    std::function<void()> function_to_repeat;
    Tetrion::Controls current_control;
    Tetrion::Controls previous_control = Tetrion::Controls::None;

    tetrion_ = std::make_shared<Tetrion>();

    while (!quit) {
      SDL_Event event;

      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          quit = true;
          break;
        }
        current_control = Tetrion::Controls::None;
        switch (event.type) {
          case SDL_KEYDOWN:
            current_control = TranslateKeyboardCommands(event, button_pressed);
            button_pressed = true;
            break;
          case SDL_JOYBUTTONDOWN:
            current_control = TranslateJoystickCommands(event);
            button_pressed = true;
            break;
          case SDL_KEYUP:
          case SDL_JOYBUTTONUP:
            button_pressed = false;
            function_to_repeat = nullptr;
            repeat_threshold = kRepeatDelay;
            previous_control = Tetrion::Controls::None;
            break;
          case SDL_JOYDEVICEADDED:
            AttachJoystick(event.jbutton.which);
            std::cout << "Joystick device added: " << event.jbutton.which << std::endl;
            break;
          case SDL_JOYDEVICEREMOVED:
            DetachJoystick(event.jbutton.which);
            std::cout << "Joystick device removed: " << event.jbutton.which << std::endl;
            break;
        }
        if (Tetrion::Controls::None == current_control) {
          continue;
        }
        switch (current_control) {
          case Tetrion::Controls::SoftDrop:
            Repeat(button_pressed, Tetrion::Controls::SoftDrop, previous_control, function_to_repeat, repeat_counter);
            break;
          case Tetrion::Controls::Left:
            Repeat(button_pressed, Tetrion::Controls::Left, previous_control, function_to_repeat, repeat_counter);
            break;
          case Tetrion::Controls::Right:
            Repeat(button_pressed, Tetrion::Controls::Right, previous_control, function_to_repeat, repeat_counter);
            break;
          case Tetrion::Controls::RotateCounterClockwise:
            tetrion_->GameControl(Tetrion::Controls::RotateCounterClockwise);;
            break;
          case Tetrion::Controls::RotateClockwise:
            tetrion_->GameControl(Tetrion::Controls::RotateClockwise);
            break;
          case Tetrion::Controls::HardDrop:
            tetrion_->GameControl(Tetrion::Controls::HardDrop);
            break;
          case Tetrion::Controls::Hold:
            tetrion_->GameControl(Tetrion::Controls::Hold);
            break;
          case Tetrion::Controls::Start:
            tetrion_->NewGame();
            break;
          case Tetrion::Controls::Pause:
            tetrion_->Pause();
            break;
          default:
            break;
        }
        if (kAutoRepeatControls.count(current_control) == 0) {
          function_to_repeat = nullptr;
          previous_control = Tetrion::Controls::None;
        }
      }
      if (button_pressed && (time_in_ms() - repeat_counter) >= repeat_threshold) {
        if (function_to_repeat) {
          function_to_repeat();
        }
        repeat_threshold = kRepeatInterval;
        repeat_counter = time_in_ms();
      }
      tetrion_->Update(delta_timer.GetDelta());
    }
  }

 private:
  int joystick_index_ = -1;
  SDL_Joystick* joystick_ = nullptr;
  std::shared_ptr<Tetrion> tetrion_ = nullptr;
};

int main(int, char*[]) {
  Combatris combatris;

  combatris.Play();

  return 0;
}
