#include "game/panes/multi_player.h"

using namespace network;

namespace {

const double kUpdateInterval = 0.3;
const int kSpaceBetweenBoxes = 11;

} // namespace

MultiPlayer::MultiPlayer(SDL_Renderer* renderer, Events& events, const std::shared_ptr<Assets>& assets)
      : Pane(renderer, kX, kY, assets), events_(events) {}

void MultiPlayer::Update(const Event& event) {
  if (!multiplayer_controller_) {
    return;
  }
  switch (event.type()) {
    case Event::Type::CalculatedScore:
      accumulator_.AddScore(event.score_);
      break;
    case Event::Type::ScoringData:
      accumulator_.AddScore(event.lines_dropped_);
      accumulator_.AddLines(event.lines_cleared());
      break;
    case Event::Type::LevelUp:
      accumulator_.SetLevel(event.current_level_);
      break;
    case Event::Type::BattleSendLines:
      multiplayer_controller_->SendUpdate(event.lines_);
      break;
    case Event::Type::GameOver:
      multiplayer_controller_->SendUpdate(GameState::GameOver);
      break;
    default:
      break;
  }
}

void MultiPlayer::Reset() {
  if (!multiplayer_controller_) {
    return;
  }
  auto& player = players_.at(our_host_name());

  player->Reset();
}

void MultiPlayer::Render(double delta_time) {
  if (multiplayer_controller_) {
    multiplayer_controller_->Dispatch();
  }
  Pane::SetDrawColor(renderer_, Color::Black);
  Pane::FillRect(renderer_, kX, kY, kMultiPlayerPaneWidth, kMultiPlayerPaneHeight);

  int offset = 0;

  for (const auto& player : score_board_) {
    player->Render((kBoxHeight + kSpaceBetweenBoxes) * offset, our_host_name() == player->name());
    offset++;
  }
  ticks_ += delta_time;
  if (ticks_ >= kUpdateInterval) {
    ticks_ = 0.0;
    if (accumulator_.is_dirty_) {
      multiplayer_controller_->SendUpdate(accumulator_.lines_, accumulator_.score_, accumulator_.level_);
      accumulator_.is_dirty_  = false;
    }
  }
}

// ListenerInterface

void MultiPlayer::GotJoin(const std::string& name) {
  if (name != our_host_name()) {
    multiplayer_controller_->Join(game_state_);
  }
  score_board_.push_back(players_.insert(std::make_pair(name, std::make_shared<Player>(renderer_, name, assets_))).first->second);
}

void MultiPlayer::GotLeave(const std::string& name) {
  auto it = std::find_if(score_board_.begin(), score_board_.end(), [&name](const auto& e) { return name == e->name(); });

  score_board_.erase(it);
  players_.erase(name);
}

void MultiPlayer::GotResetCountDown(const std::string& name) {
  if (GameState::Playing == game_state_) {
    return;
  }
  const auto& our_name = our_host_name();

  if (name == our_name) {
    events_.Push(Event::Type::BattleResetCountDown);
    return;
  }
  auto& player = players_.at(our_name);

  if (GameState::Waiting == player->state()) {
    events_.Push(Event::Type::BattleResetCountDown);
  }
}

void MultiPlayer::GotStartGame() { events_.Push(Event::Type::NextTetromino); }

void MultiPlayer::GotUpdate(const std::string& name, size_t lines, size_t score, size_t level, GameState state) {
  auto& player = players_.at(name);

  if (name == our_host_name()) {
    game_state_ = (GameState::None == game_state_) ? game_state_ : state;
  }
  if (player->Update(lines, score, level, state)) {
    std::sort(score_board_.begin(), score_board_.end(), [](const auto& a, const auto& b) { return a->score() > b->score(); });
  }
}

void MultiPlayer::GotLines(const std::string& name, size_t lines) {
  if (name == our_host_name()) {
    return;
  }
  auto event = Event(Event::Type::BattleGotLines);

  event.lines_ = lines;
  events_.Push(event);
}
