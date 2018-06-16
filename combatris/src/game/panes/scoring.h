#pragma once

#include "game/panes/level.h"

class Scoring final : public Pane, public EventListener {
 public:
  enum class LinesClearedMode { Normal, Marathon };

  Scoring(SDL_Renderer* renderer, const std::shared_ptr<Assets>& assets, Events& events) : Pane(renderer, kMatrixEndX + kMinoWidth, kMatrixStartY - kMinoHeight, assets), events_(events) { Reset(); }

  virtual void Reset() override {
    level_ = start_level_;
    score_ = 0;
    ClearCounters();
    DisplayScore(score_);
  }

  inline void ClearCounters() { combo_counter_ = b2b_counter_ = 0; }

  virtual void Update(const Event& event) override;

  virtual void Render(double) override { RenderCopy(score_texture_.get(), rc_); }

 protected:
  void DisplayScore(int score);

  void UpdateEvents(int score, ComboType combo_type, int lines_to_send, int lines_to_clear, const Event& event);

  std::tuple<int, int, ComboType, int, int> Calculate(const Event& event);

 private:
  Events& events_;
  int score_ = 0;
  int combo_counter_ = 0;
  int b2b_counter_ = 0;
  SDL_Rect rc_;
  UniqueTexturePtr score_texture_ = nullptr;
  CampaignRuleType rule_type_ = CampaignRuleType::Normal;
  int level_ = 1;
  int start_level_ = 1;
};
