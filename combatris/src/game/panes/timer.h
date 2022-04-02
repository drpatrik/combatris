#pragma once

#include "utility/timer.h"
#include "game/events.h"
#include "game/panes/pane.h"

class Timer final : public Pane, public EventListener {
 public:
  Timer (SDL_Renderer* renderer, const std::shared_ptr<Assets>& assets, Events& events);

  virtual void Update(const Event& event) override;

  virtual void Reset() override;

  virtual void Render(double) override;

 private:
  Events& events_;
  std::unique_ptr<utility::TimerInterface> timer_;
  utility::UniqueTexturePtr timer_texture_;
  SDL_Rect timer_texture_rc_;
  CampaignType campaign_type_ = CampaignType::None;
};
