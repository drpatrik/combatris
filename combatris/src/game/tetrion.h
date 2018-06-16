#pragma once

#include "game/campaign.h"
#include "game/animation.h"

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
    F1,
    F2,
    F3,
    F4,
    F5,
    Plus,
    Minus,
    Quit,
    DebugSendLine
  };

  Tetrion();

  Tetrion(const Tetrion&) = delete;

  Tetrion(const Tetrion&&) = delete;

  ~Tetrion() noexcept;

  void NewGame() { events_.Push(Event::Type::NewGame); }

  void Pause() {
    if (IsBattleCampaign(*campaign_) || !tetromino_in_play_) {
      return;
    }
    if (!game_paused_) {
      unpause_pressed_ = false;
      game_paused_ = true;
      events_.Push(Event::Type::Pause);
    } else if (!unpause_pressed_) {
      unpause_pressed_ = true;
    }
  }

  void ResetCountDown();

  void HandleGameSettings(Controls control_pressed);

  void GameControl(Controls control_pressed, int lines = 0);

  void Update(double delta_timer);

 protected:
  template<class T, class ...Args>
  void AddAnimation(Args&&... args) { animations_.push_back(std::make_shared<T>(std::forward<Args>(args)...)); }

  void HandleNextTetromino(TetrominoSprite::State state, Events& events);

  void EventHandler(Events& events);

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
  std::shared_ptr<TetrominoGenerator> tetromino_generator_;
  std::deque<std::shared_ptr<Animation>> animations_;
};
