#pragma once

#include "game/events.h"
#include "game/panes/pane.h"

class HighScore final : public TextPane, public EventSink {
 public:
  HighScore(SDL_Renderer* renderer, const std::shared_ptr<Assets>& assets) :
      TextPane(renderer,  kMatrixEndX + kMinoWidth + 8, (kMatrixStartY - kMinoHeight) + 428, "HIGH", assets) {
    SetCaptionOrientation(TextPane::Orientation::Left);
    Read();
    SetCenteredText(highscore_, Color::SteelGray, Normal35);
  }

  virtual ~HighScore() noexcept { Save(); }

  virtual void Reset() override { score_ = 0; }

  virtual void Update(const Event& event) override {
    if (event.Is(Event::Type::CalculatedScore) || event.Is(Event::Type::ScoringData)) {
      score_ += event.score_;
      score_ += event.lines_dropped_;
      if (score_ > highscore_) {
        highscore_ = score_;
      }
    }
    SetCenteredText(highscore_, Color::SteelGray, Normal35);
  }

 protected:
  void Read();
  void Save() const;

 private:
  int score_ = 0;
  int highscore_ = 0;
};
