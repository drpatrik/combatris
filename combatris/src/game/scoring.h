#pragma once

#include "game/level.h"

class Scoring {
 public:
  explicit Scoring(Level& level) : level_(level) {}

  void NewGame() {
    score_ = 0;
    level_.NewGame();
  }

  void Update(const Event& event) {
    if (event.type() == Event::Type::SoftDrop) {
      score_ += event.lines_dropped_;
    } else if (event.type() == Event::Type::HardDrop) {
      score_ += event.lines_dropped_ * 2;
    } else {
      const std::vector<int> kScoreForLines = { 40, 100, 300, 1200 };

      switch (event.bonus_move_) {
        case Event::BonusMove::None:
          break;
        case Event::BonusMove::T_Spin:
          std::cout << "T-Spin" << std::endl;
          break;
        case Event::BonusMove::T_SpinMini:
          std::cout << "T-Spin mini" << std::endl;
          break;

      }

      score_ += (kScoreForLines.at(event.lines_cleared() - 1) * (level_.level()));
    }
  }

  void Render() {
    // Will be moved into Pane at some point in time
    level_.RenderText(10, 10, Font::Normal, "Score: ", Color::White);
    level_.RenderText(84,  10, Font::Normal, std::to_string(score_), Color::White);
  }

 private:
  Level& level_;
  int score_ = 0;
};
