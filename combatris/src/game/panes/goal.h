#pragma once

#include "game/panes/level.h"

class Goal final : public TextPane, public EventListener {
 public:
  static const int kYOffs = 300;

  Goal(SDL_Renderer* renderer, const std::shared_ptr<Assets>& assets, Events& events)
      : TextPane(renderer, kMatrixStartX - kMinoWidth - (kBoxWidth + kSpace), (kMatrixStartY - kMinoHeight) + kYOffs,
                 "GOAL", assets), events_(events) { Reset(); }

  virtual void Reset() override {
    level_ = start_level_;
    goal_ = (CampaignType::Sprint == campaign_type_) ? kSprintGoal : level_ * 5;
    SetCenteredText(goal_);
  }

  virtual void Update(const Event& event) override {
    if (!IsIn(event, { Event::Type::LinesCleared, Event::Type::LevelUp, Event::Type::SetStartLevel, Event::Type::SetCampaign })) {
      return;
    }
    switch (event) {
      case Event::Type::SetCampaign:
        campaign_type_ = event.campaign_type();
        goal_ = IsSprintCampaign(campaign_type_) ? kSprintGoal : level_ * 5;
        break;
      case Event::Type::SetStartLevel:
        level_ = start_level_ = event.value1_;
        goal_ = IsSprintCampaign(campaign_type_) ? kSprintGoal : level_ * 5;
        break;
      case Event::Type::LinesCleared:
        goal_ = std::max(goal_ - event.value1_, 0);
        if (IsSprintCampaign(campaign_type_) && goal_ == 0) {
          events_.Push(Event::Type::SprintClearedAll);
        }
        break;
      case Event::Type::LevelUp:
        level_ = event.value1_;
        goal_ = IsSprintCampaign(campaign_type_) ? goal_ : level_ * 5;
        break;
      default:
        break;
    }
    SetCenteredText(goal_);
  }

 private:
  int goal_ = 0;
  int level_ = 1;
  int start_level_ = 1;
  Events& events_;
  CampaignType campaign_type_ = CampaignType::None;
};
