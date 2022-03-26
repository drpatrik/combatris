#pragma once

#include <SDL.h>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

namespace utility {

const int kNoController = -1;

class GameController final {
 public:
  class Callback {
   public:
    virtual ~Callback() noexcept {}
    virtual void AddGameController(int index, const std::string& name) = 0;
    virtual void RemoveGameController(int index) = 0;
  };

  GameController(const std::string& path, Callback* callback = nullptr) : callback_(callback) {
    if (SDL_GameControllerAddMappingsFromFile((path + "gamecontrollerdb.txt").c_str()) == -1) {
      std::cout << "Warning: Failed to load game controller mappings: " << SDL_GetError() << std::endl;
    }
    SDL_GameControllerEventState(SDL_ENABLE);
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
  }

  ~GameController() { Detach(gamecontroller_index_); }

  void AddCallback(Callback* callback) { callback_ = callback; }

  void Attach(int index) {
    if (nullptr != game_controller_ || kNoController == index || SDL_IsGameController(index) == 0) {
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

  void Detach(int index) {
    if (nullptr == game_controller_ || kNoController == index || index != gamecontroller_index_) {
      return;
    }
    std::cout << "Game controller detached: " << gamecontroller_name_ << std::endl;
    SDL_GameControllerClose(game_controller_);
    game_controller_ = nullptr;
    gamecontroller_index_ = kNoController;
    gamecontroller_name_ = "";
  }

  std::vector<std::string> GetGameControllersAsList() const {
    std::vector<std::string> lst;

    std::transform(game_controllers_.begin(), game_controllers_.end(), std::back_inserter(lst), [](const auto& p) { return p.second;} );

    return lst;
  }

  const std::map<int, std::string>& GetGameControllers() const { return game_controllers_; }

  void HandleEvents(SDL_Event& event) {
    switch (event.type) {
      case SDL_JOYDEVICEADDED:
        if (SDL_IsGameController(event.jbutton.which) == 0) {
          DisplayJoystickInfo(event.jbutton.which);
          break;
        }
        event.cbutton.which = event.jbutton.which;
        [[fallthrough]];
      case SDL_CONTROLLERDEVICEADDED:
        if (SDL_IsGameController(event.cbutton.which) == 0) {
          return;
        }
        if (game_controllers_.count(event.cbutton.which) > 0) {
          break;
        }
        game_controllers_.emplace(event.cbutton.which, SDL_GameControllerNameForIndex(event.cbutton.which));
        if (nullptr != callback_) {
          callback_->AddGameController(event.cbutton.which, SDL_GameControllerNameForIndex(event.cbutton.which));
        }
        break;
      case SDL_JOYDEVICEREMOVED:
        event.cbutton.which = event.jbutton.which;
        [[fallthrough]];
      case SDL_CONTROLLERDEVICEREMOVED:
        game_controllers_.erase(event.cbutton.which);
        if (nullptr != callback_) {
          callback_->RemoveGameController(event.cbutton.which);
        }
        Detach(event.cbutton.which);
        break;
    }
  }

 protected:
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

 private:
  int gamecontroller_index_ = kNoController;
  std::string gamecontroller_name_;
  SDL_GameController* game_controller_ = nullptr;
  Callback* callback_;
  std::map<int, std::string> game_controllers_;
};

}  // namespace utility
