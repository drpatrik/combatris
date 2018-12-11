#pragma once

#include "utility/timer.h"
#include "game/events.h"
#include "game/panes/pane.h"

class HighScore final : public TextPane, public EventListener {
 public:
  HighScore(SDL_Renderer* renderer, const std::shared_ptr<Assets>& assets) :
      TextPane(renderer,  kMatrixEndX + kMinoWidth + kSpace, (kMatrixStartY - kMinoHeight) + 428, "HIGH SCORE", assets) {
    SetCaptionOrientation(TextPane::Orientation::Left);
    Read();
  }

  virtual ~HighScore() noexcept { Save(); }

  virtual void Reset() override { score_ = 0; }

  virtual void Update(const Event& event) override {
    if (!IsIn(event, { Event::Type::CalculatedScore, Event::Type::DropScoreData, Event::Type::NewTime, Event::Type::SetCampaign })) {
      return;
    }
    if (event.Is(Event::Type::NewTime)) {
      best_time_ = std::min(event.value_, best_time_);
    } else if (event.Is(Event::Type::SetCampaign)) {
      campaign_type_ = event.campaign_type();
    } else {
      score_ += event.score_;
      score_ += event.value_; // Lines dropped
      if (score_ > highscore_) {
        highscore_ = score_;
      }
    }
    SetText();
  }

  void SetText() {
    if (IsSprintCampaign(campaign_type_)) {
      SetCenteredText(utility::FormatTimeMMSSHS(best_time_), Color::SteelGray, Normal25);
    } else {
      SetCenteredText(highscore_, Color::SteelGray, Normal35);
    }
  }

 protected:
  void Read();

  void Save() const;

 private:
  int score_ = 0;
  int highscore_ = 0;
  size_t best_time_ = 600000; // 10 minutes
  CampaignType campaign_type_ = CampaignType::None;
};
