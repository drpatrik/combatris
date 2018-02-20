#pragma once

#include "game/panes/level.h"

class Scoring final : public TextPane, public EventSink {
 public:
  Scoring(SDL_Renderer* renderer, const std::shared_ptr<Assets>& assets, const std::shared_ptr<Level>& level) :
      TextPane(renderer, kMatrixStartX - kMinoWidth - (kBoxWidth + 8), (kMatrixStartY - kMinoHeight) + 150, "SCORE", assets),
      level_(level), events_(level->GetEvents()) { SetCenteredText(0); }

  virtual void Reset() override {
    score_ = 0;
    ClearCounters();
    SetCenteredText(0);
  }

  inline void ClearCounters() { combo_counter_ = b2b_counter_ = 0; }

  virtual void Update(const Event& event) override;

 protected:
  void UpdateEvents(int score, ComboType combo_type, const Event& event);

  std::tuple<int, int, ComboType> Calculate(const Event& event);

 private:
  std::shared_ptr<Level> level_;
  Events& events_;
  int score_ = 0;
  int combo_counter_ = 0;
  int b2b_counter_ = 0;
};
