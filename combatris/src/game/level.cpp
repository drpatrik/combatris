#include "game/level.h"
#include "game/asset_manager.h"

#include <vector>

namespace {

// Gravity is expressed in unit G, where 1G = 1 cell per frame, and 0.1G = 1 cell per 10 frames
struct LevelData {
  LevelData(int level, double gravity, double lock_delay) : level_(level), gravity_(gravity), lock_delay_(lock_delay) {}

  int level_;
  double gravity_;
  double lock_delay_;
};

std::vector<LevelData> kLevelData = {
  LevelData(1, 0.01667, 0.5),
  LevelData(2, 0.021017, 0.5),
  LevelData(3, 0.026977, 0.5),
  LevelData(4, 0.035256, 0.5),
  LevelData(5, 0.04693, 0.5),
  LevelData(6, 0.06361, 0.5),
  LevelData(7, 0.0879, 0.5),
  LevelData(8, 0.1236, 0.5),
  LevelData(9, 0.1775, 0.5),
  LevelData(10, 0.2598, 0.5),
  LevelData(11, 0.388, 0.5),
  LevelData(12, 0.59, 0.5),
  LevelData(13, 0.92, 0.5),
  LevelData(14, 1.46, 0.5),
  LevelData(15, 2.36, 0.5)
};

const int kLevelUp = 10;

} // namespace

void Level::ResetTime() {
  int index = level_ < static_cast<int>(kLevelData.size()) ? level_ :  kLevelData.size() - 1;

  wait_time_ = (1.0 / kLevelData.at(index).gravity_) / 60.0;
  lock_delay_ = kLevelData.at(index).lock_delay_;
  time_ = 0;
}

bool Level::WaitForMoveDown(double time_delta) {
  time_ += time_delta;
  if (time_ >= wait_time_) {
    time_ = 0.0;
    return true;
  }
  return false;
}

bool Level::WaitForLockDelay(double time_delta) {
  time_ += time_delta;
  if  (time_ >= lock_delay_) {
    return true;
  }
  return false;
}

void Level::Update(const Event& event) {
  int lines_cleared = event.lines_cleared_.size();

  if (0 == lines_cleared) {
    return;
  }
  total_lines_ += lines_cleared;
  lines_this_level_ += lines_cleared;
  if (lines_this_level_ >= kLevelUp) {
    events_.Push(EventType::LevelUp);
    lines_this_level_ = 0;
    level_++;
    ResetTime();
  }
}

void Level::Render() {
  // Will be moved into Pane at some point in time
  RenderText(x_, y_ + 50 , Font::Normal, "Level: ", Color::White);
  RenderText(x_ + 74,  y_ +50, Font::Normal, std::to_string(level_ + 1), Color::White);
  RenderText(x_, y_ + 100, Font::Normal, "Lines: ", Color::White);
  RenderText(x_ + 74,  y_ + 100, Font::Normal, std::to_string(total_lines_), Color::White);
}
