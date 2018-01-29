#pragma once

#include "game/assets.h"
#include "game/events.h"

class Level {
 public:
  Level(SDL_Renderer* renderer, Events& events, const std::shared_ptr<Assets>& assets, int x, int y)
      : renderer_(renderer), events_(events), assets_(assets), x_(x), y_(y) { ResetTime(); }

  bool Wait(double time_delta, bool floor_reached) { return (floor_reached) ? WaitForLockDelay(time_delta) : WaitForMoveDown(time_delta); }

  void Release() { time_ += 60.0; }

  void Update(const Event& event);

  int level() const { return level_ + 1; }

  void NewGame() {
    time_ = 0.0;
    wait_time_ = 0.0;
    lock_delay_ = 0.0;
    level_ = 0;
    total_lines_ = 0;
    lines_this_level_ = 0;
  }

  void ResetTime();

  void Render();

  void RenderText(int x, int y, Font font, const std::string& text, Color text_color) const {
    ::RenderText(renderer_, x, y, assets_->GetFont(font), text, text_color);
  }

 protected:
  bool WaitForMoveDown(double time_delta);

  bool WaitForLockDelay(double time_delta);

 private:
  SDL_Renderer* renderer_;
  Events& events_;
  std::shared_ptr<Assets> assets_;
  int x_;
  int y_;
  double time_ = 0.0;
  double wait_time_ = 0.0;
  double lock_delay_ = 0.0;
  int level_ = 0;
  int total_lines_ = 0;
  int lines_this_level_ = 0;
};
