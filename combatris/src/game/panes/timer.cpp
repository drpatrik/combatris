#include "game/panes/timer.h"

namespace {

using UniqueTexturePtr = utility::UniqueTexturePtr;
using Color = utility::Color;

const int kTimesUpSoon = 15;
const int kGameTime = 120;

std::pair<UniqueTexturePtr, SDL_Rect> CreateTimerTexture(SDL_Renderer* renderer, const Assets& assets,
                                                         const std::string& text, Color color = Color::White) {
  auto [texture, width, height] = CreateTextureFromText(renderer, assets.GetFont(ObelixPro40), text, color);

  return std::make_pair(std::move(texture), SDL_Rect{ kMatrixStartX, 5, width, height });
}

} // namespace

Timer::Timer(SDL_Renderer* renderer, const std::shared_ptr<Assets>& assets, Events& events)
    : Pane(renderer, 0, 0, assets), events_(events), timer_(kGameTime) { Reset(); }

void ::Timer::Update(const Event& event) {
  switch (event.type()) {
    case Event::Type::SetCampaign:
      campaign_type_ = ToCampaignType(event.value_);
      break;
    case Event::Type::GameOver:
      timer_.Stop();
      break;
    case Event::Type::NextTetromino:
      if (IsBattleCampaign(campaign_type_) && !timer_.IsStarted()) {
        timer_.Start();
      }
      break;
    case Event::Type::MultiplayerResetCountDown:
      std::tie(timer_texture_, timer_texture_rc_) = CreateTimerTexture(renderer_, *assets_, timer_.FormatTime(kGameTime));
      break;
    default:
      break;
  }
}

void ::Timer::Reset() {
   std::tie(timer_texture_, timer_texture_rc_) = CreateTimerTexture(renderer_, *assets_, timer_.FormatTime(kGameTime));
}

void ::Timer::Render(double) {
  if (!IsBattleCampaign(campaign_type_)) {
    return;
  }
  if (timer_.IsStarted()) {
    auto [updated, time_in_sec] = timer_.GetTimeInSeconds();

    if (updated) {
      auto color = (time_in_sec <= kTimesUpSoon) ? Color::Red : Color::White;

      std::tie(timer_texture_, timer_texture_rc_) = CreateTimerTexture(renderer_, *assets_, timer_.FormatTime(time_in_sec), color);
      if (timer_.IsZero()) {
        timer_.Stop();
        events_.Push(Event::Type::GameOver);
      }
    }
  }
  Pane::RenderCopy(timer_texture_.get(), timer_texture_rc_);
}
