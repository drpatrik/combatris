#include "game/panes/multi_player.h"

using namespace network;

namespace {

const int kMaxPlayers = 9;
const double kUpdateInterval = 0.5;
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
      if (event.value_ > 0) {
        accumulator_.AddLinesSent(event.value_);
        multiplayer_controller_->SendUpdate(event.value_);
        events_.Push(Event::Type::BattleSendLines, event.value_);
      }
      break;
    case Event::Type::DropScoreData:
      accumulator_.AddScore(event.value_);
      break;
    case Event::Type::LinesCleared:
      accumulator_.AddLines(event.lines_cleared());
      break;
    case Event::Type::LevelUp:
      accumulator_.SetLevel(event.value_);
      break;
    case Event::Type::GameOver:
      multiplayer_controller_->SendUpdate(GameState::GameOver);
      break;
    case Event::Type::BattleNextTetrominoInPlay:
      if (!got_lines_from_.empty()) {
        got_lines_from_.pop_front();
      }
      break;
    case Event::Type::BattleKnockOut:
      multiplayer_controller_->SendUpdate(got_lines_from_.front());
      got_lines_from_.pop_front();
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
      multiplayer_controller_->SendUpdate(accumulator_.lines_, accumulator_.lines_sent_, accumulator_.score_,
                                          accumulator_.ko_, accumulator_.level_);
      accumulator_.is_dirty_  = false;
    }
  }
  multiplayer_controller_->Dispatch();
}

// ListenerInterface

bool MultiPlayer::GotJoin(const std::string& name, uint64_t host_id)  {
  if (score_board_.size() >= kMaxPlayers) {
    std::cout << "Combatris support - " << kMaxPlayers << " players." << std::endl;
    return false;
  }
  if (!IsUs(host_id)) {
    multiplayer_controller_->Join(game_state_);
  }
  score_board_.push_back(
      players_.insert(std::make_pair(host_id, std::make_shared<Player>(renderer_, name, host_id, assets_)))
          .first->second);

  return true;
}

void MultiPlayer::GotLeave(uint64_t host_id) {
  if (players_.count(host_id) == 0) {
    return;
  }
  auto it = std::find_if(score_board_.begin(), score_board_.end(), [host_id](const auto& e) { return host_id == e->host_id(); });

  score_board_.erase(it);
  players_.erase(host_id);
}

void MultiPlayer::GotNewGame(uint64_t host_id) {
  if (IsUs(host_id)) {
    accumulator_.Reset();
    events_.Push(Event::Type::BattleResetCountDown);
  } else if (GameState::Waiting == game_state_) {
    events_.Push(Event::Type::BattleResetCountDown);
  }
  auto& player = players_.at(host_id);

  player->Reset();
}

void MultiPlayer::GotStartGame() { events_.Push(Event::Type::NextTetromino); }

void MultiPlayer::GotUpdate(uint64_t host_id, int lines, int lines_sent, int score, int ko, int level, GameState state) {
  if (IsUs(host_id)) {
    game_state_ = (GameState::None == state) ? game_state_ : state;
  }
  auto& player = players_.at(host_id);

  if (player->Update(lines, lines_sent, score, ko, level, state)) {
    std::sort(score_board_.begin(), score_board_.end(), [](const auto& a, const auto& b) {
      if (a->ko() != b->ko()) {
        return a->ko() > b->ko();
      }
      return a->lines_sent() > b->lines_sent();
    });
  }
}

void MultiPlayer::GotLines(uint64_t host_id, int lines) {
  got_lines_from_.push_back(host_id);
  events_.Push(Event::Type::BattleGotLines, lines);
}

void MultiPlayer::GotPlayerKnockedOut() {
  accumulator_.AddKnockOut(1);
  events_.Push(Event::Type::BattleYouDidKO);
}
