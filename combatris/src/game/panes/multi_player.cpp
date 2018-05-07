#include "game/panes/multi_player.h"

using namespace network;

namespace {

const int kMaxPlayers = 9;
const int kGameTime = 120;
const double kUpdateInterval = 0.250;
const int kSpaceBetweenBoxes = 11;

std::pair<UniqueTexturePtr, SDL_Rect> CreateTimerTexture(SDL_Renderer* renderer, const Assets& assets,
                                                         const std::string& text, Color color = Color::White) {
  auto [texture, width, height] = CreateTextureFromText(renderer, assets.GetFont(TimerFont), text, color);

  return std::make_pair(std::move(texture), SDL_Rect{ kMatrixStartX, 5, width, height });
}

} // namespace

MultiPlayer::MultiPlayer(SDL_Renderer* renderer, Events& events, const std::shared_ptr<Assets>& assets)
    : Pane(renderer, kX, kY, assets), events_(events), timer_(kGameTime) {
  std::tie(timer_texture_, timer_texture_rc_) = CreateTimerTexture(renderer, *assets, timer_.FormatTime(kGameTime));
}

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
      timer_.Stop();
      multiplayer_controller_->SendUpdate(GameState::GameOver);
      break;
    case Event::Type::NextTetromino:
      if (!timer_.IsStarted()) {
        timer_.Start();
      }
      break;
    case Event::Type::BattleNextTetrominoSuccessful:
      if (!got_lines_from_.empty()) {
        got_lines_from_.pop_front();
      }
      break;
    case Event::Type::BattleKnockedOut:
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
  if (timer_.IsStarted()) {
    auto [updated, time_in_sec] = timer_.GetTimeInSeconds();

    if (updated) {
      auto color = (time_in_sec <= 15) ? Color::Red : Color::White;

      std::tie(timer_texture_, timer_texture_rc_) = CreateTimerTexture(renderer_, *assets_, timer_.FormatTime(time_in_sec), color);
      if (timer_.IsZero()) {
        timer_.Stop();
        events_.Push(Event::Type::GameOver);
      }
    }
  }
  Pane::SetDrawColor(renderer_, Color::Black);
  Pane::FillRect(renderer_, kX, kY, kMultiPlayerPaneWidth, kMultiPlayerPaneHeight);

  int offset = 0;

  for (const auto& player : score_board_) {
    player->Render((kBoxHeight + kSpaceBetweenBoxes) * offset, IsUs(player->host_id()));
    offset++;
  }
  Pane::RenderCopy(timer_texture_.get(), timer_texture_rc_);

  ticks_progess_update_ += delta_time;
  if (ticks_progess_update_ >= kUpdateInterval) {
    ticks_progess_update_ = 0.0;
    if (accumulator_.IsDirty()) {
      multiplayer_controller_->SendUpdate(accumulator_.lines_, accumulator_.lines_sent_, accumulator_.score_,
                                          accumulator_.ko_, accumulator_.level_);
    }
  }
  multiplayer_controller_->Dispatch();
}

void MultiPlayer::SortScoreBoard() {
  std::sort(score_board_.begin(), score_board_.end(), [](const auto& a, const auto& b) {
    if (a->ko() != b->ko()) {
      return a->ko() > b->ko();
    }
    return a->lines_sent() > b->lines_sent();
  });
#if !defined(NDEBUG)
  std::cout << "-----\n";
  for (const auto& p : score_board_) {
    std::cout << p->name() << " Ko: " << p->ko() << ", LS: " << p->lines_sent() << "\n";
  }
#endif
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
    std::tie(timer_texture_, timer_texture_rc_) = CreateTimerTexture(renderer_, *assets_, timer_.FormatTime(kGameTime));
    for (auto& player : score_board_) {
      player->Reset();
    }
    SortScoreBoard();
    events_.Push(Event::Type::BattleResetCountDown);
  } else if (GameState::Waiting == game_state_) {
    events_.Push(Event::Type::BattleResetCountDown);
  }
}

void MultiPlayer::GotStartGame() { events_.Push(Event::Type::BattleWaitForPlayers); }

void MultiPlayer::GotUpdate(uint64_t host_id, int lines, int lines_sent, int score, int ko, int level, GameState state) {
  if (IsUs(host_id)) {
    game_state_ = (GameState::None == state) ? game_state_ : state;
  }
  auto& player = players_.at(host_id);

  if (player->Update(lines, lines_sent, score, ko, level, state)) {
    SortScoreBoard();
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
