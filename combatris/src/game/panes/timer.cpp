#include "game/panes/timer.h"

namespace {

using UniqueTexturePtr = utility::UniqueTexturePtr;
using Color = utility::Color;

const int kTimesUpSoon = 15;
const int kGameTimeBattle = 120;
const int kGameTimeUltra = 180;

std::pair<UniqueTexturePtr, SDL_Rect> CreateTimerTexture(SDL_Renderer* renderer, const Assets& assets,
                                                         const std::string& text, Color color = Color::White) {
  auto [texture, width, height] = CreateTextureFromText(renderer, assets.GetFont(ObelixPro40), text, color);

  return std::make_pair(std::move(texture), SDL_Rect{ kMatrixStartX, 5, width, height });
}

} // namespace

Timer::Timer(SDL_Renderer* renderer, const std::shared_ptr<Assets>& assets, Events& events)
    : Pane(renderer, 0, 0, assets), events_(events) {}

void ::Timer::Update(const Event& event) {
  switch (event.type()) {
    case Event::Type::SetCampaign:
      campaign_type_ = event.campaign_type();
      timer_in_use_ = false;
      if (IsBattleCampaign(campaign_type_) || CampaignType::Ultra == campaign_type_) {
        timer_in_use_ = true;
        timer_ = std::make_unique<utility::Timer>(0);
      } else if (IsSprintCampaign(campaign_type_)) {
        timer_in_use_ = true;
        timer_ = std::make_unique<utility::Clock>();
      }
      Reset();
      break;
    case Event::Type::GameOver:
      timer_->Stop();
      break;
    case Event::Type::SprintClearedAll:
      events_.Push(Event::Type::NewTime, timer_->GetTime().second);
      events_.Push(Event::Type::GameOver);
      break;
    case Event::Type::NextTetromino:
      if (timer_in_use_ && !timer_->IsStarted()) {
        timer_->Start();
      }
      break;
    case Event::Type::MultiplayerResetCountDown:
      Reset();
      break;
    default:
      break;
  }
}

void ::Timer::Reset() {
  if (!timer_in_use_) {
    return;
  }
  size_t t = 0;

  if (IsBattleCampaign(campaign_type_) || IsUltraCampaign(campaign_type_)) {
    t = IsUltraCampaign(campaign_type_) ? kGameTimeUltra : kGameTimeBattle;
  }
  std::tie(timer_texture_, timer_texture_rc_) = CreateTimerTexture(renderer_, *assets_, timer_->FormatTime(t));
  timer_->Set(t);
}

void ::Timer::Render(double) {
  if (!timer_in_use_) {
    return;
  }
  if (timer_->IsStarted()) {
    auto [updated, t] = timer_->GetTime();

    if (updated) {
      if (IsSprintCampaign(campaign_type_)) {
        std::tie(timer_texture_, timer_texture_rc_) = CreateTimerTexture(renderer_, *assets_, timer_->FormatTime(t), Color::White);
      } else {
        auto color = (t <= kTimesUpSoon) ? Color::Red : Color::White;

        std::tie(timer_texture_, timer_texture_rc_) = CreateTimerTexture(renderer_, *assets_, timer_->FormatTime(t), color);

        if (timer_->IsZero()) {
          timer_->Stop();
          events_.Push(Event::Type::GameOver);
        }
      }
    }
  }
  Pane::RenderCopy(timer_texture_.get(), timer_texture_rc_);
}