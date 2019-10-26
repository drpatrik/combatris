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

  void DisplayJoystickInfo(int index) {
    auto js = SDL_JoystickOpen(index);

    if (nullptr == js) {
      std::cout << "Unknown joystick - unable to find information: " << SDL_GetError() << std::endl;
      return;
    }
    char guid_str[1024];
    SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(js), guid_str, sizeof(guid_str));

    const auto name = SDL_JoystickName(js);

    std::cout << guid_str << ", " << name << " - not found in database" << std::endl;;

    SDL_JoystickClose(js);
  }

  void AttachController(int index) {
    if (nullptr != game_controller_ || SDL_IsGameController(index) == 0) {
      if (nullptr == game_controller_) {
        DisplayJoystickInfo(index);
      }
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
    if (event.key.repeat > 0) {
      return Tetrion::Controls::None;
    }
    const auto code = event.key.keysym.scancode;

    if (SDL_SCANCODE_LEFT == code) {
      return Tetrion::Controls::Left;
    } else if (SDL_SCANCODE_RIGHT == code) {
      return Tetrion::Controls::Right;
    } if (SDL_SCANCODE_DOWN == code) {
      return Tetrion::Controls::SoftDrop;
    } else if (SDL_SCANCODE_LCTRL == code || SDL_SCANCODE_RCTRL == code || SDL_SCANCODE_Z == code) {
      return Tetrion::Controls::RotateCounterClockwise;
    } else if (SDL_SCANCODE_UP == code || SDL_SCANCODE_X == code) {
      return Tetrion::Controls::RotateClockwise;
    } else if (SDL_SCANCODE_LSHIFT == code || SDL_SCANCODE_RSHIFT == code || SDL_SCANCODE_C == code) {
      return Tetrion::Controls::Hold;
    } else if (SDL_SCANCODE_SPACE == code) {
      return Tetrion::Controls::HardDrop;
    } else if (SDL_SCANCODE_N == code) {
      return Tetrion::Controls::Start;
    } else if (SDL_SCANCODE_P == code || SDL_SCANCODE_F1 == code) {
      return Tetrion::Controls::Pause;
    } else if (SDL_SCANCODE_Q == code) {
      return Tetrion::Controls::Quit;
    } else if (code >=  SDL_SCANCODE_1 && code <=  SDL_SCANCODE_9) {
      return Tetrion::Controls::DebugSendLine;
    }
    return Tetrion::Controls::None;
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
  std::pair<Tetrion::Controls, RepeatFunc> Repeatable(int& repeat_count, int64_t& time_since_last_auto_repeat) {
    repeat_count = 0;
    time_since_last_auto_repeat = kAutoRepeatInitialDelay;
    return std::make_pair(control, [&tetrion = tetrion_]() { tetrion->GameControl(control); });
  }

  void Play() {
    bool quit = false;
    DeltaTimer delta_timer;
    int repeat_count = 0;
    int64_t time_since_last_auto_repeat = 0;
    int64_t auto_repeat_threshold = kAutoRepeatInitialDelay;
    RepeatFunc function_to_repeat;
    auto active_control = Tetrion::Controls::None;
    SDL_Event event;

    while (!quit) {
      auto control = Tetrion::Controls::None;

      while (SDL_PollEvent(&event)) {
        if (SDL_QUIT == event.type) {
          quit = true;
          break;
        }
        switch (event.type) {
          case SDL_KEYDOWN:
            control = TranslateKeyboardCommands(event);
            break;
          case SDL_CONTROLLERBUTTONDOWN:
            control = TranslateControllerCommands(event);
            break;
          case SDL_KEYUP:
            if (TranslateKeyboardCommands(event) == active_control) {
              active_control = Tetrion::Controls::None;
            }
            break;
          case SDL_CONTROLLERBUTTONUP:
            if (TranslateControllerCommands(event) == active_control) {
              active_control = Tetrion::Controls::None;
            }
            break;
          case SDL_JOYDEVICEADDED:
            if (SDL_IsGameController(event.jbutton.which) == 0) {
              DisplayJoystickInfo(event.jbutton.which);
              break;
            }
            [[fallthrough]];
          case SDL_CONTROLLERDEVICEADDED:
            AttachController(event.cbutton.which);
            break;
          case SDL_JOYDEVICEREMOVED:
            event.cbutton.which = event.jbutton.which;
            [[fallthrough]];
          case SDL_CONTROLLERDEVICEREMOVED:
            DetachController(event.cbutton.which);
            break;
        }
      }
      switch (control) {
        case Tetrion::Controls::None:
          break;
        case Tetrion::Controls::Left:
          std::tie(active_control, function_to_repeat) =
              Repeatable<Tetrion::Controls::Left>(repeat_count, time_since_last_auto_repeat);
          break;
        case Tetrion::Controls::Right:
          std::tie(active_control, function_to_repeat) =
              Repeatable<Tetrion::Controls::Right>(repeat_count, time_since_last_auto_repeat);
          break;
        case Tetrion::Controls::SoftDrop:
          std::tie(active_control, function_to_repeat) =
              Repeatable<Tetrion::Controls::SoftDrop>(repeat_count, time_since_last_auto_repeat);
          break;
        case Tetrion::Controls::Start:
          tetrion_->NewGame();
          active_control = control;
          break;
        case Tetrion::Controls::Pause:
          tetrion_->Pause();
          active_control = control;
          break;
        case Tetrion::Controls::Quit:
          quit = true;
          break;
        case Tetrion::Controls::DebugSendLine:
#if !defined(NDEBUG)
          tetrion_->GameControl(Tetrion::Controls::DebugSendLine, 9 - (SDL_SCANCODE_9 - event.key.keysym.scancode));
          active_control = control;
#endif
          break;
        default:
          tetrion_->GameControl(control);
          active_control = control;
          break;
      }
      if (kAutoRepeatControls.count(active_control) && (time_in_ms() - time_since_last_auto_repeat) >= auto_repeat_threshold) {
        function_to_repeat();
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
  SDL_GameController* game_controller_ = nullptr;
  std::shared_ptr<Tetrion> tetrion_ = nullptr;
};

int main(int, char *[]) {
  Combatris combatris;

  combatris.Play();

  return 0;
}
