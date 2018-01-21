#pragma once

#include "game/asset_manager.h"

class Level {
 public:
  Level(SDL_Renderer* renderer, const std::shared_ptr<AssetManager>& asset_manager, int x, int y)
      : renderer_(renderer), asset_manager_(asset_manager), x_(x), y_(y) { ResetTime(); }

  bool Wait(double time_delta, bool floor_reached) { return (floor_reached) ? WaitForLockDelay(time_delta) : WaitForMoveDown(time_delta); }

  void Release() { time_ += 60.0; }

  void LinesCleared(int lines_cleared);

  void NewGame() {
    time_ = 0.0;
    wait_time_ = 0.0;
    lock_delay_ = 0.0;
    level_ = 0;
    score_ = 0;
    total_lines_ = 0;
    lines_this_level_ = 0;
  }

  void ResetTime();

  void Render();

 protected:
  bool WaitForMoveDown(double time_delta);

  bool WaitForLockDelay(double time_delta);

 void RenderText(int x, int y, Font font, const std::string& text, Color text_color) const {
    ::RenderText(renderer_, x, y, asset_manager_->GetFont(font), text, text_color);
  }

 private:
  SDL_Renderer* renderer_;
  std::shared_ptr<AssetManager> asset_manager_;
  int x_;
  int y_;
  double time_ = 0.0;
  double wait_time_ = 0.0;
  double lock_delay_ = 0.0;
  int level_ = 0;
  int score_ = 0;
  int total_lines_ = 0;
  int lines_this_level_ = 0;
};
