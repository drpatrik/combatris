#pragma once

#include "game/level.h"

class Scoring final : public Pane {
 public:
  Scoring(SDL_Renderer* renderer, const std::shared_ptr<Assets>& assets, Level& level) :
      Pane(renderer, 10, 10, assets), level_(level) {}

  void NewGame() {
    score_ = 0;
    ClearCounter();
    level_.NewGame();
  }

  void ClearCounter() {
    combo_counter_ = 0;
    b2b_tspin_counter_ = 0;
    b2b_tetris_counter_ = 0;
  }

  void Update(const Event& event) {
    const std::vector<int> kScoreForLines = { 0, 100, 300, 500, 800 };
    const std::vector<int> kScoreForTSpin = { 400, 800, 1200, 500 };
    const std::vector<int> kB2BScoreForTSpin = { 0, 1200, 1800, 500 };

    int base_score = 0;
    int combo_score = 0;

    switch (event.tspin_type_) {
      case TSpinType::None:
        base_score = kScoreForLines.at(event.lines_cleared());
        if (event.lines_cleared() == 4) {
          if (++b2b_tetris_counter_ > 1) {
            combo_score = 1200;
            std::cout << "B2B Tetris" << std::endl;
          }
        } else if (event.lines_cleared() > 0) {
          if (++combo_counter_ > 1) {
            combo_score = (event.lines_cleared() == 1) ? 20 : 50;
          }
        } else {
          ClearCounter();
        }
        break;
      case TSpinType::TSpin:
        base_score = kScoreForTSpin.at(event.lines_cleared());
        if (++b2b_tspin_counter_ > 1) {
          combo_score = kB2BScoreForTSpin.at(event.lines_cleared());
        }
        std::cout << "T-Spin" << std::endl;
        break;
      case TSpinType::TSpinMini:
        base_score = 100;
        // No extra score but will send garbage
        std::cout << "T-Spin mini" << std::endl;
        break;
    }
    score_ += event.lines_dropped_;
    score_ += (base_score * level_.level()) + (combo_score * level_.level());
  }

  virtual void Render() const override {
    RenderText(x_, y_, Font::Normal, "Score: ", Color::White);
    RenderText(x_ + 74,  y_ , Font::Normal, std::to_string(score_), Color::White);
  }

 private:
  Level& level_;
  int score_ = 0;
  int combo_counter_ = 0;
  int b2b_tspin_counter_ = 0;
  int b2b_tetris_counter_ = 0;
};
