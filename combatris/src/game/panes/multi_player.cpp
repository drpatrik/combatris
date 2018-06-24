#include "game/panes/multi_player.h"

using namespace network;

namespace {

const int kMaxPlayers = 6;
const int kGameTime = 120;
const double kUpdateInterval = 0.250;

std::pair<UniqueTexturePtr, SDL_Rect> CreateTimerTexture(SDL_Renderer* renderer, const Assets& assets,
                                                         const std::string& text, Color color = Color::White) {
  auto [texture, width, height] = CreateTextureFromText(renderer, assets.GetFont(ObelixPro40), text, color);

  return std::make_pair(std::move(texture), SDL_Rect{ kMatrixStartX, 5, width, height });
}

MatrixState GetMatrixState(const std::shared_ptr<Matrix>& m) {
  MatrixState matrix_state;

  int i = 0;
  const auto& matrix = m->data();

  for (int row = kVisibleRowStart; row < kVisibleRowEnd; ++row) {
    const auto& col_vec = matrix[row];

    for (int col = kVisibleColStart; col < kVisibleColEnd; col +=2) {
      auto e1 = (col_vec[col] >= kGhostAddOn) ? 0 : col_vec[col];
      auto e2 = (col_vec[col + 1] >= kGhostAddOn) ? 0 : col_vec[col + 1];

      matrix_state[i] = static_cast<uint8_t>((e1 << 4) | e2);
      i++;
    }
  }

  return matrix_state;
}

} // namespace

MultiPlayer::MultiPlayer(SDL_Renderer* renderer, const std::shared_ptr<Matrix>& matrix, Events& events,
                         const std::shared_ptr<Assets>& assets)
    : Pane(renderer, kX, kY, assets), matrix_(matrix), events_(events), timer_(kGameTime) {
  std::tie(timer_texture_, timer_texture_rc_) = CreateTimerTexture(renderer, *assets, timer_.FormatTime(kGameTime));
}

void MultiPlayer::Update(const Event& event) {
  if (!multiplayer_controller_) {
    return;
  }
  switch (event.type()) {
    case Event::Type::SetStartLevel:
      start_level_ = event.value_;
      break;
    case Event::Type::SetCampaign:
      campaign_type_ = ToCampaignType(event.value_);
      break;
    case Event::Type::CalculatedScore:
      accumulator_.AddScore(event.score_);
      if (IsBattleCampaign(campaign_type_) && event.value_ > 0) {
        multiplayer_controller_->SendUpdate(event.value_);
        events_.Push(Event::Type::BattleSendLines, event.value_);
      }
      break;
    case Event::Type::DropScoreData:
      accumulator_.AddScore(event.value_);
      break;
    case Event::Type::LinesCleared:
      accumulator_.AddLines(event.value_);
      break;
    case Event::Type::LevelUp:
      accumulator_.SetLevel(event.value_);
      break;
    case Event::Type::GameOver:
      timer_.Stop();
      multiplayer_controller_->SendUpdate(GameState::GameOver);
      break;
    case Event::Type::NextTetromino:
      if (IsBattleCampaign(campaign_type_) && !timer_.IsStarted()) {
        timer_.Start();
      }
      break;
    case Event::Type::MultiplayerStartGame:
      multiplayer_controller_->StartGame();
      break;
    case Event::Type::BattleNextTetrominoSuccessful:
      if (!IsBattleCampaign(campaign_type_)) {
        break;
      }
      if (!got_lines_from_.empty()) {
        got_lines_from_.pop_front();
      }
      break;
    case Event::Type::BattleKnockedOut:
      if (!IsBattleCampaign(campaign_type_)) {
        break;
      }
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

  int x_offset = 0;
  int y_offset = 0;

  for (const auto& player : score_board_) {
    player->Render((kBoxWidth + kSpaceBetweenBoxes) * x_offset, (kBoxHeight + kSpaceBetweenBoxes) * y_offset, IsUs(player->host_id()));
    x_offset++;
    if (x_offset > 1) {
      y_offset++;
      x_offset = 0;
    }
  }
  if (IsBattleCampaign(campaign_type_)) {
    Pane::RenderCopy(timer_texture_.get(), timer_texture_rc_);
  }
  ticks_progess_update_ += delta_time;
  if (ticks_progess_update_ >= kUpdateInterval) {
    ticks_progess_update_ = 0.0;
    if (matrix_->IsDirty()) {
      multiplayer_controller_->SendUpdate(accumulator_.lines_, accumulator_.score_, accumulator_.level_, GetMatrixState(matrix_));
    }
  }
  multiplayer_controller_->Dispatch();
}

void MultiPlayer::SortScoreBoard() {
  if (IsBattleCampaign(campaign_type_)) {
    std::sort(score_board_.begin(), score_board_.end(), [](const auto& a, const auto& b) {
      if (GameState::Idle == b->state()) {
        return true;
      }
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
  } else {
    std::sort(score_board_.begin(), score_board_.end(),
              [](const auto& a, const auto& b) {
      if (GameState::Idle == b->state()) {
        return true;
      }
      return a->score() > b->score();
    });
#if !defined(NDEBUG)
    std::cout << "-----\n";
    for (const auto& p : score_board_) {
      std::cout << p->name() << " Score: " << p->score() << "\n";
    }
#endif
  }
}

// ListenerInterface

bool MultiPlayer::GotJoin(const std::string& name, uint64_t host_id)  {
  if (score_board_.size() >= kMaxPlayers) {
    std::cout << "Combatris support - " << kMaxPlayers << " players.\n";
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
    accumulator_.Reset(start_level_);
    std::tie(timer_texture_, timer_texture_rc_) = CreateTimerTexture(renderer_, *assets_, timer_.FormatTime(kGameTime));
    for (auto& player : score_board_) {
      player->Reset();
    }
    events_.Push(Event::Type::MultiplayerResetCountDown);
  } else if (GameState::Waiting == game_state_) {
    events_.Push(Event::Type::MultiplayerResetCountDown);
  }
}

void MultiPlayer::GotStartGame() {
  for (auto& player : score_board_) {
    if (GameState::GameOver == player->state()) {
      player->SetState(GameState::Idle);
    }
  }
  SortScoreBoard();
  if (1 == players_.size() || CanStartGame()) {
    events_.Push(Event::Type::NextTetromino);
  } else {
    events_.Push(Event::Type::MultiplayerWaitForPlayers);
  }
}

void MultiPlayer::GotNewState(uint64_t host_id, network::GameState state) {
  if (IsUs(host_id)) {
    game_state_ = (GameState::None == state) ? game_state_ : state;
  }
  auto& player = players_.at(host_id);

  player->SetState(state);
}

void MultiPlayer::GotProgressUpdate(uint64_t host_id, int lines, int score, int level, const MatrixState& state) {
  auto& player = players_.at(host_id);

  score =  (IsBattleCampaign(campaign_type_)) ? 0 : score;
  if (player->ProgressUpdate(lines, score, level)) {
    SortScoreBoard();
  }
  player->SetMatrixState(state);
}

void MultiPlayer::GotLines(uint64_t host_id, int lines) {
  if (!IsBattleCampaign(campaign_type_)) {
    return;
  }
  if (!IsUs(host_id)) {
    got_lines_from_.push_back(host_id);
    events_.Push(Event::Type::BattleGotLines, lines);
  }
  auto& player = players_.at(host_id);

  player->AddLinesSent(lines);
  SortScoreBoard();
}

void MultiPlayer::GotPlayerKnockedOut(uint64_t host_id) {
  if (IsUs(host_id)) {
    events_.Push(Event::Type::BattleYouDidKO);
  }
  auto& player = players_.at(host_id);

  player->AddKO(1);
  SortScoreBoard();
}
