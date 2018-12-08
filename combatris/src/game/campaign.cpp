#include "game/campaign.h"

namespace {

const SDL_Rect kSinglePlayerRC =  { 0, 0, kWidth, kHeight };
const SDL_Rect kBattleRC =  { 0, 0, kWidth + kMultiPlayerWidthAddOn, kHeight };

void SetupCampaignWindow(SDL_Window* window, SDL_Renderer* renderer, bool is_single_player) {
  const auto& rc = is_single_player ? kSinglePlayerRC : kBattleRC;

  SDL_RenderSetLogicalSize(renderer, rc.w, rc.h);
  SDL_SetWindowSize(window, rc.w, rc.h);
}

inline const SDL_Rect& GetWindowRc(bool is_single_player) {
  return (is_single_player) ? kSinglePlayerRC : kBattleRC;
}

void RenderWindowBackground(SDL_Renderer* renderer, const SDL_Rect& rc) {
  SDL_SetRenderDrawColor(renderer, 1, 40, 135, 255);
  SDL_RenderFillRect(renderer, &rc);
}

std::string ToString(CampaignType type) {
  switch (type) {
    case CampaignType::None:
      return "ERROR NO CAMPAIGN SET";
    case CampaignType::Combatris:
      return "Combatris";
    case CampaignType::Marathon:
      return "Marathon";
    case CampaignType::Sprint:
      return "Sprint";
    case CampaignType::Ultra:
      return "Ultra";
    case CampaignType::Battle:
      return "Battle";
  }
  return "";
}

} // namespace

Campaign::Campaign(SDL_Window* window, SDL_Renderer* renderer, Events& events, const std::shared_ptr<Assets>& assets,
                   const std::shared_ptr<Matrix>& matrix)
    : window_(window), renderer_(renderer), events_(events), assets_(assets), matrix_(matrix) {
  level_ = std::make_shared<Level>(renderer_, 150, events_, assets_);
  AddListener(level_.get());
  tetromino_generator_ = std::make_shared<TetrominoGenerator>(matrix_, level_, events_, assets_);
  scoring_ = std::make_unique<Scoring>(renderer_, assets_, events_);
  AddListener(scoring_.get());
  timer_ = std::make_unique<::Timer>(renderer_, assets_, events_);
  AddListener(timer_.get());
  high_score_ = std::make_unique<HighScore>(renderer_, assets_);
  AddListener(high_score_.get());
  next_queue_ = std::make_unique<NextQueue>(renderer_, tetromino_generator_, assets_);
  hold_queue_ = std::make_unique<HoldQueue>(renderer_, tetromino_generator_, assets_);
  AddListener(hold_queue_.get());
  goal_ = std::make_unique<Goal>(renderer_, 300, assets_, events_);
  AddListener(goal_.get());
  total_lines_ = std::make_unique<TotalLines>(renderer_, 578, assets_);
  AddListener(total_lines_.get());
  lines_sent_ = std::make_unique<LinesSent>(renderer_, 450, assets_);
  AddListener(lines_sent_.get());
  moves_ = std::make_unique<Moves>(renderer_, assets_);
  AddListener(moves_.get());
  knockout_ = std::make_unique<Knockout>(renderer_, assets_);
  AddListener(knockout_.get());
  multi_player_ = std::make_shared<MultiPlayer>(renderer_, matrix_, events_, assets_);
  AddListener(multi_player_.get());
  AddListener(this);
}

void Campaign::Update(const Event& event) {
  if (!event.Is(Event::Type::MenuSetModeAndCampaign)) {
    return;
  }
  auto title = ToString(event.campaign_type());

  if (event.mode_type() != mode_type_) {
    mode_type_ = event.mode_type();
    switch (mode_type_) {
      case ModeType::SinglePlayer:
        multi_player_->Disable();
        break;
      case ModeType::MultiPlayer:
        multi_player_->Enable();
        break;
      default:
        break;
    }
  }
  if (ModeType::MultiPlayer == mode_type_) {
    title = "Multiplayer - " + title + " (" + multi_player_->our_host_name() + " )";
  }
  SetupCampaignWindow(window_, renderer_, IsSinglePlayer());
  SDL_SetWindowTitle(window_, title.c_str());
  SetupCampaign(event.campaign_type());

}

void Campaign::Render(double delta_time) {
  RenderWindowBackground(renderer_, GetWindowRc(IsSinglePlayer()));
  std::for_each(panes_.begin(), panes_.end(), [delta_time](const auto& pane) { pane->Render(delta_time); });
}

Event Campaign::PreprocessEvent(const Event& event) {
  std::for_each(event_listeners_.begin(), event_listeners_.end(), [&event](const auto& r) { r->Update(event); });
  return event;
}

void Campaign::SetupCampaign(CampaignType type) {
  if (type == campaign_type_) {
    return;
  }
  campaign_type_ = type;
  panes_.clear();
  panes_.push_back(matrix_.get());
  panes_.push_back(level_.get());
  panes_.push_back(next_queue_.get());
  panes_.push_back(hold_queue_.get());
  panes_.push_back(moves_.get());
  panes_.push_back(multi_player_.get());

  switch (type) {
    case CampaignType::Combatris:
      panes_.push_back(scoring_.get());
      panes_.push_back(high_score_.get());
      panes_.push_back(total_lines_.get());
      break;
    case CampaignType::Marathon:
      panes_.push_back(scoring_.get());
      panes_.push_back(high_score_.get());
      panes_.push_back(goal_.get());
      break;
    case CampaignType::Sprint:
      panes_.push_back(timer_.get());
      panes_.push_back(goal_.get());
      panes_.push_back(high_score_.get());
      break;
    case CampaignType::Ultra:
      panes_.push_back(timer_.get());
      panes_.push_back(scoring_.get());
      panes_.push_back(high_score_.get());
      panes_.push_back(total_lines_.get());
      break;
    case CampaignType::Battle:
      panes_.push_back(timer_.get());
      panes_.push_back(lines_sent_.get());
      panes_.push_back(knockout_.get());
      break;
    default:
      break;
  }
  events_.Push(Event::Type::SetCampaign, campaign_type_);
}
