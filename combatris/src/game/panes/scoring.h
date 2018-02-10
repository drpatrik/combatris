#pragma once

#include "game/panes/level.h"

class Scoring final : public TextPane, public EventSink {
 public:
  Scoring(SDL_Renderer* renderer, const std::shared_ptr<Assets>& assets, const std::shared_ptr<Level>& level) :
      TextPane(renderer, kMatrixStartX - kBlockWidth - (kBoxWidth + 8), (kMatrixStartY - kBlockHeight) + 150, "SCORE", assets),
      level_(level) {
    SetCenteredText(std::to_string(0));
  }

  virtual void Reset() override {
    score_ = 0;
    ClearCounter();
    SetCenteredText(0);
  }

  void ClearCounter() {
    combo_counter_ = 0;
    b2b_tspin_counter_ = 0;
    b2b_tetris_counter_ = 0;
  }

  virtual void Update(const Event& event) override {
    if (!event.Is(Event::Type::Scoring)) {
      return;
    }
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
        } else if (!event.IsDrop()) {
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
    score_ += (base_score * level_->level()) + (combo_score * level_->level());
    TextPane::SetCenteredText(score_);
  }

 private:
  std::shared_ptr<Level> level_;
  int score_ = 0;
  int combo_counter_ = 0;
  int b2b_tspin_counter_ = 0;
  int b2b_tetris_counter_ = 0;
};
