#include "game/campaign.h"

namespace {

const SDL_Rect kSinglePlayerRC =  { 0, 0, kWidth, kHeight };
const SDL_Rect kBattleRC =  { 0, 0, kWidth + kMultiPlayerWidthAddOn, kHeight };

void SetupCampaignWindow(SDL_Window* window, SDL_Renderer* renderer, Campaign::Type campaign) {
  const auto& rc = (campaign == Campaign::Type::SinglePlayer) ? kSinglePlayerRC : kBattleRC;

  SDL_RenderSetLogicalSize(renderer, rc.w, rc.h);
  SDL_SetWindowSize(window, rc.w, rc.h);
}

inline const SDL_Rect& GetWindowRc(Campaign::Type campaign) {
  return (Campaign::Type::SinglePlayer == campaign) ? kSinglePlayerRC : kBattleRC;
}

void RenderWindowBackground(SDL_Renderer* renderer, const SDL_Rect& rc) {
  SDL_SetRenderDrawColor(renderer, 1, 40, 135, 255);
  SDL_RenderFillRect(renderer, &rc);
}

std::string ToString(Campaign::Type type) {
  switch (type) {
    case Campaign::Type::SinglePlayer:
      return "COMBATRIS - Single Player";
    case Campaign::Type::Battle:
      return "COMBATRIS - Battle";
  }

  return "";
}

} // namespace

Campaign::Campaign(SDL_Renderer* renderer, Events& events, const std::shared_ptr<Assets>& assets, const std::shared_ptr<Matrix>& matrix) : renderer_(renderer), events_(events), assets_(assets), matrix_(matrix) {
  AddPane(matrix_.get());
  level_ = std::make_shared<Level>(renderer_, events_, assets_);
  AddPane(level_.get());
  tetromino_generator_ = std::make_shared<TetrominoGenerator>(matrix_, level_, events_, assets_);
  scoring_ = std::make_unique<Scoring>(renderer_, assets_, level_);
  AddPane(scoring_.get());
  high_score_ = std::make_unique<HighScore>(renderer_, assets_);
  AddPane(high_score_.get());
  next_queue_ = std::make_unique<NextQueue>(renderer_, tetromino_generator_, assets_);
  AddPane(next_queue_.get());
  hold_queue_ = std::make_unique<HoldQueue>(renderer_, tetromino_generator_, assets_);
  AddPane(hold_queue_.get());
  total_lines_ = std::make_unique<TotalLines>(renderer_, assets_);
  AddPane(total_lines_.get());
  moves_ = std::make_unique<Moves>(renderer_, assets_);
  AddPane(moves_.get());
  multi_player_ = std::make_shared<MultiPlayer>(renderer_, matrix_, events_, assets_);
  AddPane(multi_player_.get());
}

void Campaign::Set(SDL_Window* window) {
  std::string title;

  if (Type::SinglePlayer == type_) {
    multi_player_->Enable();
    type_ = Type::Battle;
    title = ToString(type_) + " (" + multi_player_->our_host_name() + " )";
  } else {
    multi_player_->Disable();
    type_ = Type::SinglePlayer;
    title = ToString(type_);
  }
  SetupCampaignWindow(window, renderer_, type_);
  SDL_SetWindowTitle(window, title.c_str());
}

void Campaign::Render(double delta_time) {
  RenderWindowBackground(renderer_, GetWindowRc(type_));
  std::for_each(panes_.begin(), panes_.end(), [delta_time](const auto& pane) { pane->Render(delta_time); });
}

Event Campaign::PreprocessEvent(const Event& event) {
  std::for_each(event_listeners_.begin(), event_listeners_.end(), [&event](const auto& r) { r->Update(event); });
  return event;
}
