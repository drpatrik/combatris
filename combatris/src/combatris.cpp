#include "utility/timer.h"
#include "game/tetrion.h"

#include <functional>
#include <set>

namespace {

// DAS settings
const int64_t kAutoRepeatInitialDelay = 300; // milliseconds
const int64_t kAutoRepeatSubsequentDelay = 50; // milliseconds

const std::set<Tetrion::Controls> kAutoRepeatControls = {
  Tetrion::Controls::SoftDrop,
  Tetrion::Controls::Left,
  Tetrion::Controls::Right
};

} // namespace

using namespace utility;

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
    Assets::LoadGameControllerMappings();
    SDL_GameControllerEventState(SDL_ENABLE);
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    tetrion_ = std::make_shared<Tetrion>();
  }

  ~Combatris() {
    tetrion_.reset();
    DetachController(gamecontroller_index_);
    SDL_Quit();
    TTF_Quit();
  }

  inline void ResetAutoRepeat() {
    function_to_repeat_ = nullptr;
    previous_control_ = Tetrion::Controls::None;
  }

  void AttachController(int index) {
    if (nullptr != game_controller_ || SDL_IsGameController(index) == 0) {
      return;
    }
    game_controller_ = SDL_GameControllerOpen(index);
    if (nullptr == game_controller_) {
      std::cout << "Warning: Unable to open game controller! SDL Error: " << SDL_GetError() << std::endl;
      exit(-1);
    }
    gamecontroller_index_ = index;
    gamecontroller_name_ = SDL_GameControllerNameForIndex(gamecontroller_index_);
    std::cout << "Game controller attached: " << gamecontroller_name_ << std::endl;
  }

  void DetachController(int index) {
    if (nullptr == game_controller_ || index != gamecontroller_index_) {
      return;
    }
    std::cout << "Game controller detached: " << gamecontroller_name_ << std::endl;
    SDL_GameControllerClose(game_controller_);
    game_controller_ = nullptr;
    gamecontroller_index_ = -1;
    gamecontroller_name_ = "";
  }

  Tetrion::Controls TranslateKeyboardCommands(const SDL_Event& event) const {
    auto current_control = Tetrion::Controls::None;

    if (SDL_SCANCODE_LEFT == event.key.keysym.scancode) {
      current_control = Tetrion::Controls::Left;
    } else if (SDL_SCANCODE_RIGHT == event.key.keysym.scancode) {
      current_control = Tetrion::Controls::Right;
    } if (SDL_SCANCODE_DOWN == event.key.keysym.scancode) {
      current_control = Tetrion::Controls::SoftDrop;
    }
    if (0 == event.key.repeat) {
      const auto code = event.key.keysym.scancode;

      if (code == SDL_SCANCODE_LCTRL || code == SDL_SCANCODE_RCTRL || code == SDL_SCANCODE_Z) {
        current_control = Tetrion::Controls::RotateCounterClockwise;
      } else if (SDL_SCANCODE_UP == code || SDL_SCANCODE_X == code) {
        current_control = Tetrion::Controls::RotateClockwise;
      } else if (SDL_SCANCODE_LSHIFT == code || SDL_SCANCODE_RSHIFT == code || SDL_SCANCODE_C == code) {
        current_control = Tetrion::Controls::Hold;
      } else if (SDL_SCANCODE_SPACE == code) {
        current_control = Tetrion::Controls::HardDrop;
      } else if (SDL_SCANCODE_N == code) {
        current_control = Tetrion::Controls::Start;
      } else if (SDL_SCANCODE_P == code || SDL_SCANCODE_F1 == code) {
        current_control = Tetrion::Controls::Pause;
      } else if (SDL_SCANCODE_Q == code) {
        current_control = Tetrion::Controls::Quit;
      } else if (code >=  SDL_SCANCODE_1 && code <=  SDL_SCANCODE_9) {
        current_control = Tetrion::Controls::DebugSendLine;
      }
    }
    return current_control;
  }

  Tetrion::Controls TranslateControllerCommands(const SDL_Event& event) const {
    if (event.cbutton.which != gamecontroller_index_) {
      return Tetrion::Controls::None;
    }
    switch (event.cbutton.button) {
      case SDL_CONTROLLER_BUTTON_A:
        return Tetrion::Controls::RotateCounterClockwise;
      case SDL_CONTROLLER_BUTTON_B:
        return Tetrion::Controls::RotateClockwise;
      case SDL_CONTROLLER_BUTTON_BACK:
        return Tetrion::Controls::Pause;
      case SDL_CONTROLLER_BUTTON_START:
        return Tetrion::Controls::Start;
      case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        return Tetrion::Controls::Hold;
      case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        return Tetrion::Controls::Hold;
      case SDL_CONTROLLER_BUTTON_DPAD_UP:
        return Tetrion::Controls::HardDrop;
      case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        return Tetrion::Controls::SoftDrop;
      case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        return Tetrion::Controls::Left;
      case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        return Tetrion::Controls::Right;
    }
    return Tetrion::Controls::None;
  }

  template <Tetrion::Controls control>
  void Repeatable(int& repeat_count, int64_t& time_since_last_auto_repeat) {
    if (control == previous_control_) {
      return;
    }
    repeat_count = 0;
    previous_control_ = control;
    function_to_repeat_ = [&tetrion = tetrion_]() { tetrion->GameControl(control); };
    time_since_last_auto_repeat = kAutoRepeatInitialDelay;
  }

  void Play() {
    bool quit = false;
    DeltaTimer delta_timer;
    int repeat_count = 0;
    int64_t time_since_last_auto_repeat = 0;
    int64_t auto_repeat_threshold = kAutoRepeatInitialDelay;
    SDL_Event event;

    while (!quit) {
      while (SDL_PollEvent(&event)) {
        if (SDL_QUIT == event.type) {
          quit = true;
          break;
        }
        auto current_control = Tetrion::Controls::None;

        switch (event.type) {
          case SDL_KEYDOWN:
            current_control = TranslateKeyboardCommands(event);
            break;
          case SDL_CONTROLLERBUTTONDOWN:
            current_control = TranslateControllerCommands(event);
            break;
          case SDL_KEYUP:
          case SDL_CONTROLLERBUTTONUP:
            ResetAutoRepeat();
            break;
          case SDL_JOYDEVICEADDED:
            event.cbutton.which = event.jbutton.which;
          case SDL_CONTROLLERDEVICEADDED:
            AttachController(event.cbutton.which);
            break;
          case SDL_JOYDEVICEREMOVED:
            event.cbutton.which = event.jbutton.which;
          case SDL_CONTROLLERDEVICEREMOVED:
            DetachController(event.cbutton.which);
            break;
        }
        if (Tetrion::Controls::None == current_control) {
          continue;
        }
        switch (current_control) {
          case Tetrion::Controls::Left:
            Repeatable<Tetrion::Controls::Left>(repeat_count, time_since_last_auto_repeat);
            break;
          case Tetrion::Controls::Right:
            Repeatable<Tetrion::Controls::Right>(repeat_count, time_since_last_auto_repeat);
            break;
          case Tetrion::Controls::SoftDrop:
            Repeatable<Tetrion::Controls::SoftDrop>(repeat_count, time_since_last_auto_repeat);
            break;
          case Tetrion::Controls::Start:
            tetrion_->NewGame();
            break;
          case Tetrion::Controls::Pause:
            tetrion_->Pause();
            break;
          case Tetrion::Controls::Quit:
            quit = true;
            break;
          case Tetrion::Controls::DebugSendLine:
#if !defined(NDEBUG)
            tetrion_->GameControl(Tetrion::Controls::DebugSendLine, 9 - (SDL_SCANCODE_9 - event.key.keysym.scancode));
#endif
            break;
          default:
            tetrion_->GameControl(current_control);
            break;
        }
        if (!kAutoRepeatControls.count(current_control)) {
          ResetAutoRepeat();
        }
      }
      if (kAutoRepeatControls.count(previous_control_) && (time_in_ms() - time_since_last_auto_repeat) >= auto_repeat_threshold) {
        function_to_repeat_();
        auto_repeat_threshold = (0 == repeat_count) ? kAutoRepeatInitialDelay : kAutoRepeatSubsequentDelay;
        repeat_count++;
        time_since_last_auto_repeat = time_in_ms();
      }
      tetrion_->Update(delta_timer.GetDelta());
    }
  }

 private:
  int gamecontroller_index_ = -1;
  std::string gamecontroller_name_;
  std::function<void()> function_to_repeat_;
  Tetrion::Controls previous_control_ = Tetrion::Controls::None;
  SDL_GameController* game_controller_ = nullptr;
  std::shared_ptr<Tetrion> tetrion_ = nullptr;
};

int main(int, char *[]) {
  Combatris combatris;

  combatris.Play();

  return 0;
}
