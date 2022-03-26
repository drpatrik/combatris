#include "game/panes/multi_player.h"

using namespace network;

namespace {

const int kMaxPlayers = 6;
const double kUpdateInterval = 0.250;
const int kSpaceingW = kBoxWidth + kSpaceBetweenBoxes;
const int kSpacingH = kBoxHeight + kSpaceBetweenBoxes;

MatrixState GetMatrixState(const std::shared_ptr<Matrix>& m) {
  MatrixState matrix_state;

  int i = 0;
  const auto& matrix = m->data();

  for (int row = kMatrixFirstRow; row < kMatrixLastRow; ++row) {
    const auto& col_vec = matrix[row];

    for (int col = kMatrixFirstCol; col < kMatrixLastCol; col +=2) {
      auto e1 = (col_vec[col] >= kGhostAddOn) ? 0 : col_vec[col];
      auto e2 = (col_vec[col + 1] >= kGhostAddOn) ? 0 : col_vec[col + 1];

      matrix_state[i] = static_cast<uint8_t>((e1 << 4) | e2);
      i++;
    }
  }

  return matrix_state;
}

inline bool IsPlaying(GameState state) { return GameState::Playing == state; }

inline bool IsIdle(GameState state) { return GameState::Idle == state || GameState::Rejected == state; }

inline bool IsWaiting(GameState state) { return GameState::Waiting == state; }

inline bool IsGameOver(GameState state) { return GameState::GameOver == state; }

} // namespace

MultiPlayer::MultiPlayer(SDL_Renderer* renderer, const std::shared_ptr<Matrix>& matrix, Events& events,
                         const std::shared_ptr<Assets>& assets)
    : Pane(renderer, kX, kY, assets), matrix_(matrix), events_(events) {}

void MultiPlayer::Update(const Event& event) {
  if (!multiplayer_controller_) {
    return;
  }
  switch (event.type()) {
    case Event::Type::HideMultiPlayerPanel:
      is_multiplayer_panel_hidden_ = !is_multiplayer_panel_hidden_;
      break;
    case Event::Type::SetStartLevel:
      start_level_ = event.value1_;
      break;
    case Event::Type::SetCampaign:
      campaign_type_ = event.campaign_type();
      break;
    case Event::Type::CalculatedScore:
      accumulator_.AddScore(event.score());
      break;
    case Event::Type::BattleSendLines:
      multiplayer_controller_->SendLines(event.value1_);
      break;
    case Event::Type::DropScoreData:
      accumulator_.AddScore(event.value1_);
      break;
    case Event::Type::LinesCleared:
      accumulator_.AddLines(event.value1_);
      break;
    case Event::Type::LevelUp:
      accumulator_.SetLevel(event.value1_);
      break;
    case Event::Type::GameOver:
      multiplayer_controller_->SendState(GameState::GameOver);
      break;
    case Event::Type::PlayerRejected:
      multiplayer_controller_->SendState(GameState::Rejected);
      break;
    case Event::Type::MultiplayerStartGame:
      multiplayer_controller_->StartGame();
      break;
    case Event::Type::NewTime:
      multiplayer_controller_->SendTime(event.value2_);
      break;
    case Event::Type::BattleKnockedOut:
      if (!IsBattleCampaign(campaign_type_)) {
        break;
      }
      multiplayer_controller_->SendKnockedoutBy(event.value2_);
      break;
    default:
      break;
  }
}

void MultiPlayer::Render(double delta_time) {
  if (!multiplayer_controller_) {
    return;
  }
  if (!is_multiplayer_panel_hidden_) {
    Pane::SetDrawColor(renderer_, Color::Black);
    Pane::FillRect(renderer_, kX, kY, kMultiPlayerPaneWidth, kMultiPlayerPaneHeight);

    int x_offset = 0;
    int y_offset = 0;

    for (const auto& player : score_board_) {
      player->Render(kSpaceingW * x_offset, kSpacingH * y_offset);
      x_offset++;
      if (x_offset > 1) {
        y_offset++;
        x_offset = 0;
      }
    }
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
      if (IsIdle(b->state())) {
        return true;
      }
      if (a->ko() != b->ko()) {
        return a->ko() > b->ko();
      }
      return a->lines_sent() > b->lines_sent();
    });
  } else if (IsSprintCampaign(campaign_type_)) {
    std::sort(score_board_.begin(), score_board_.end(),
              [](const auto& a, const auto& b) {
      if (IsIdle(b->state())) {
        return true;
      }
      if (a->time() != b->time()) {
        if (b->time() == 0) {
          return true;
        }
        return a->time() < b->time();
      }
      return a->lines() > b->lines();
    });
  } else {
    std::sort(score_board_.begin(), score_board_.end(),
              [](const auto& a, const auto& b) {
      if (IsIdle(b->state())) {
        return true;
      }
      return a->score() > b->score();
    });
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
      players_.insert(std::make_pair(host_id, std::make_shared<Player>(renderer_, name, host_id, IsUs(host_id), assets_)))
          .first->second);

  return true;
}

void MultiPlayer::GotLeave(uint64_t host_id) {
  if (0 == players_.count(host_id)) {
    return;
  }
  GotNewState(host_id, GameState::GameOver);

  auto it = std::find_if(score_board_.begin(), score_board_.end(), [host_id](const auto& e) { return host_id == e->host_id(); });

  score_board_.erase(it);
  players_.erase(host_id);
}

void MultiPlayer::GotNewGame(uint64_t host_id, CampaignType type, uint64_t seed) {
  vote_.Add(host_id, type, seed);

  if (IsUs(host_id)) {
    accumulator_.Reset(start_level_);
    for (auto& player : score_board_) {
      player->Reset();
    }
    events_.Push(Event::Type::MultiplayerResetCountDown);
  } else if (IsWaiting(game_state_)) {
    events_.Push(Event::Type::MultiplayerResetCountDown);
  } else {
    if (IsGameOver(game_state_)) {
      auto it = players_.find(multiplayer_controller_->our_host_id());

      if (it != players_.end()) {
        game_state_ = GameState::Idle;
        it->second->SetState(GameState::Idle);
        events_.Push(Event::Type::ShowSplashScreen);
      }
    }
    if (IsIdle(game_state_)) {
      SortScoreBoard();
    }
  }
  auto& player = players_.at(host_id);

  player->SetCampaignType(type);
}

void MultiPlayer::GotStartGame() {
  for (auto& player : score_board_) {
    if (IsGameOver(player->state())) {
      player->SetState(GameState::Idle);
    }
  }
  SortScoreBoard();
  if (auto result = vote_.Cast(multiplayer_controller_->our_host_id())) {
    events_.Push(Event::Type::MultiPlayerSetSeed, static_cast<size_t>(*result));
    events_.Push(Event::Type::NextTetromino);
  } else {
    events_.Push(Event::Type::PlayerRejected);
  }
  vote_.Clear();
}

void MultiPlayer::GotNewState(uint64_t host_id, network::GameState state) {
  if (0 == players_.count(host_id)) {
    return;
  }
  if (IsUs(host_id)) {
    game_state_ = state;
  }
  auto& player = players_.at(host_id);

  player->SetState(state);
  if (!IsIdle(game_state_) && !IsWaiting(game_state_)) {
    if (IsGameOver(state) && CanPressNewGame()) {
      SortScoreBoard();
      auto it = std::find_if(score_board_.begin(), score_board_.end(), [this](const auto& p) { return IsUs(p->host_id()); });

      events_.Push(Event::Type::MultiplayerCampaignOver, static_cast<size_t>(std::distance(score_board_.begin(), it)));
    }
  }
}

void MultiPlayer::GotProgressUpdate(uint64_t host_id, int lines, int score, int level, const MatrixState& state) {
  if (0 == players_.count(host_id)) {
    return;
  }
  auto& player = players_.at(host_id);

  score = (IsBattleCampaign(campaign_type_) || IsSprintCampaign(campaign_type_)) ? -1 : score;
  lines = IsSprintCampaign(campaign_type_) ? std::min(lines, kSprintGoal) : lines;
  if (player->ProgressUpdate(lines, score, level)) {
    SortScoreBoard();
  }
  player->SetMatrixState(state);
}

void MultiPlayer::GotLines(uint64_t host_id, int lines) {
  if (0 == players_.count(host_id) || !IsPlaying(game_state_) || !IsBattleCampaign(campaign_type_)) {
    return;
  }
  if (!IsUs(host_id)) {
    events_.Push(Event::Type::BattleGotLines, lines, host_id);
  }
  auto& player = players_.at(host_id);

  player->AddLinesSent(lines);
  SortScoreBoard();
}

void MultiPlayer::GotPlayerKnockedOut(uint64_t host_id) {
  if (0 == players_.count(host_id)) {
    return;
  }
  if (IsUs(host_id)) {
    events_.Push(Event::Type::BattleYouDidKO);
  }
  auto& player = players_.at(host_id);

  player->AddKO(1);
  SortScoreBoard();
}

void MultiPlayer::GotTime(uint64_t host_id, uint64_t time) {
  if (!IsSprintCampaign(campaign_type_) || 0 == players_.count(host_id)) {
    return;
  }
  auto& player = players_.at(host_id);

  player->SetTime(time);
  SortScoreBoard();
}
