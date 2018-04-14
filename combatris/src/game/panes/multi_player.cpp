#include "game/panes/multi_player.h"

using namespace network;

namespace {

const int kMaxPlayers = 9;
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
      accumulator_.AddLinesSent(event.lines_);
      multiplayer_controller_->SendUpdate(event.lines_);
      break;
    case Event::Type::GameOver:
      multiplayer_controller_->SendUpdate(GameState::GameOver);
      break;
    default:
      break;
  }
}

void MultiPlayer::Render(double delta_time) {
  if (!multiplayer_controller_) {
    return;
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
      multiplayer_controller_->SendUpdate(accumulator_.lines_, accumulator_.lines_sent_, accumulator_.score_, accumulator_.level_);
      accumulator_.is_dirty_  = false;
    }
  }
  multiplayer_controller_->Dispatch();
}

// ListenerInterface

bool MultiPlayer::GotJoin(const std::string& name)  {
  if (score_board_.size() >= kMaxPlayers) {
    std::cout << "Combatris support - " << kMaxPlayers << " players." << std::endl;
    return false;
  }
  if (name != our_host_name()) {
    multiplayer_controller_->Join(game_state_);
  }
  score_board_.push_back(players_.insert(std::make_pair(name, std::make_shared<Player>(renderer_, name, assets_))).first->second);

  return true;
}

void MultiPlayer::GotLeave(const std::string& name) {
  auto it = std::find_if(score_board_.begin(), score_board_.end(), [&name](const auto& e) { return name == e->name(); });

  score_board_.erase(it);
  players_.erase(name);
}

void MultiPlayer::GotNewGame(const std::string& name) {
  auto& player = players_.at(name);

  if (name == our_host_name()) {
    accumulator_.Reset();
    events_.Push(Event::Type::BattleResetCountDown);
  } else if (GameState::Waiting == game_state_) {
    events_.Push(Event::Type::BattleResetCountDown);
  }
  player->Reset();
}

void MultiPlayer::GotStartGame() { events_.Push(Event::Type::NextTetromino); }

void MultiPlayer::GotUpdate(const std::string& name, int lines, int lines_sent, int score, int level, GameState state) {
  auto& player = players_.at(name);

  if (name == our_host_name()) {
    game_state_ = (GameState::None == state) ? game_state_ : state;
  }
  if (player->Update(lines, lines_sent, score, level, state)) {
    std::sort(score_board_.begin(), score_board_.end(), [](const auto& a, const auto& b) { return a->score() > b->score(); });
  }
}

void MultiPlayer::GotLines(const std::string& name, int lines) {
  if (name == our_host_name()) {
    return;
  }
  auto event = Event(Event::Type::BattleGotLines);

  event.lines_ = lines;
  events_.Push(event);
}
