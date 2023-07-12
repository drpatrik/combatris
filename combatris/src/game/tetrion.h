#pragma once

#include "game/campaign.h"
#include "game/animation.h"
#include "utility/game_controller.h"

class Tetrion final {
 public:
  enum class Controls {
    None,
    RotateClockwise,
    RotateCounterClockwise,
    SoftDrop,
    HardDrop,
    Left,
    Right,
    Hold,
    Pause,
    Start,
    HideMultiplayerPanel,
    Quit,
    DebugSendLine,
    Up = HardDrop,
    UpKeyBoard = RotateClockwise,
    Down = SoftDrop
  };

  Tetrion();

  Tetrion(const Tetrion&) = delete;

  Tetrion(const Tetrion&&) = delete;

  ~Tetrion() noexcept;

  void NewGame() { events_.Push(Event::Type::NewGame); }

  void Pause() {
    if (!campaign_->IsSinglePlayer() || !tetromino_in_play_) {
      return;
    }
    if (!game_paused_) {
      unpause_pressed_ = false;
      game_paused_ = true;
      events_.Push(Event::Type::Pause);
    } else {
      unpause_pressed_ = true;
    }
  }

  void ResetCountDown();

  void GameControl(Controls control_pressed, int lines = 0);

  void HandleGameControllerEvents(SDL_Event& event) { game_controller_->HandleEvents(event); }

  void Update(double delta_timer);

 protected:
  template<class T, class ...Args>
  void AddAnimation(Args&&... args) { animations_.push_back(std::make_shared<T>(std::forward<Args>(args)...)); }

  void HandleMenu(Controls control_pressed);

  void HandleTetrominoStates(TetrominoSprite::State state, Events& events);

  void EventHandler(Events& events, double delta_time);

  void Render(double delta_timer);

 private:
  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  std::shared_ptr<TetrominoSprite> tetromino_in_play_;

  Events events_;
  bool game_paused_ = false;
  bool unpause_pressed_ = false;
  std::shared_ptr<Assets> assets_;
  std::shared_ptr<Matrix> matrix_;
  std::shared_ptr<Campaign> campaign_;
  std::shared_ptr<HoldQueue> hold_queue_;
  std::shared_ptr<MultiPlayer> multi_player_;
  std::shared_ptr<ReceivingQueue> receiving_queue_;
  std::shared_ptr<TetrominoGenerator> tetromino_generator_;
  std::deque<std::shared_ptr<Animation>> animations_;
  std::shared_ptr<CombatrisMenu> combatris_menu_;
  std::shared_ptr<utility::GameController> game_controller_;
};
