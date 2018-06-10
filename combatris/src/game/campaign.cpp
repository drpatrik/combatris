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
    case CampaignType::Tetris:
      return "COMBATRIS";
    case CampaignType::Marathon:
      return "COMBATRIS - Marathon";
    case CampaignType::MultiPlayerVS:
      return "COMBATRIS - Multiplayer Vs.";
    case CampaignType::MultiPlayerMarathon:
      return "COMBATRIS - Multiplayer Marathon";
    case CampaignType::MultiPlayerBattle:
      return "COMBATRIS - Multiplayer Battle";
  }
  return "";
}

} // namespace

Campaign::Campaign(SDL_Renderer* renderer, Events& events, const std::shared_ptr<Assets>& assets, const std::shared_ptr<Matrix>& matrix) : renderer_(renderer), events_(events), assets_(assets), matrix_(matrix) {
  level_ = std::make_shared<Level>(renderer_, 150, events_, assets_);
  AddListener(level_.get());
  tetromino_generator_ = std::make_shared<TetrominoGenerator>(matrix_, level_, events_, assets_);
  scoring_ = std::make_unique<Scoring>(renderer_, assets_, level_);
  AddListener(scoring_.get());
  high_score_ = std::make_unique<HighScore>(renderer_, assets_);
  AddListener(high_score_.get());
  next_queue_ = std::make_unique<NextQueue>(renderer_, tetromino_generator_, assets_);
  hold_queue_ = std::make_unique<HoldQueue>(renderer_, tetromino_generator_, assets_);
  AddListener(hold_queue_.get());
  goal_ = std::make_unique<Goal>(renderer_, 300, assets_, level_);
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
}

void Campaign::Set(SDL_Window* window, CampaignType type) {
  if (type == type_) {
    return;
  }
  type_ = type;

  std::string title = ToString(type_);

  switch (type_) {
    case CampaignType::Tetris:
      multi_player_->Disable();
      break;
    case CampaignType::Marathon:
      multi_player_->Disable();
      break;
    case CampaignType::MultiPlayerVS:
      multi_player_->Enable();
      title += " (" + multi_player_->our_host_name() + " )";
      break;
    case CampaignType::MultiPlayerMarathon:
      multi_player_->Enable();
      title += " (" + multi_player_->our_host_name() + " )";
      break;
    case CampaignType::MultiPlayerBattle:
      multi_player_->Enable();
      title += " (" + multi_player_->our_host_name() + " )";
      break;
    default:
      break;
  }
  SetupCampaignWindow(window, renderer_, IsSinglePlayer());
  SDL_SetWindowTitle(window, title.c_str());
  SetupCampaign(type_);
  events_.Push(Event::Type::SetCampaign, type_);
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
  panes_.clear();
  panes_.push_back(matrix_.get());
  panes_.push_back(level_.get());
  panes_.push_back(next_queue_.get());
  panes_.push_back(hold_queue_.get());
  panes_.push_back(moves_.get());
  panes_.push_back(multi_player_.get());

  switch (type) {
    case CampaignType::Tetris:
    case CampaignType::MultiPlayerVS:
      panes_.push_back(scoring_.get());
      panes_.push_back(high_score_.get());
      panes_.push_back(total_lines_.get());
      break;
    case CampaignType::Marathon:
    case CampaignType::MultiPlayerMarathon:
      panes_.push_back(scoring_.get());
      panes_.push_back(high_score_.get());
      panes_.push_back(goal_.get());
      break;
    case CampaignType::MultiPlayerBattle:
      panes_.push_back(lines_sent_.get());
      panes_.push_back(knockout_.get());
      break;
    default:
      break;
  }
}
