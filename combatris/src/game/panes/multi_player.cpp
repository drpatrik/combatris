#include "game/panes/multi_player.h"

using namespace network;

namespace {

const int kX = kMatrixEndX + kMinoWidth + (kSpace * 4) + TextPane::kBoxWidth;
const int kY = kMatrixStartY - kMinoHeight;

// Player Statistic
const int kBoxX = kX;
const int kBoxY = kY;
const int kLineThinkness = 2;
const int kBoxWidth = 220;
const int kBoxHeight = 100;
const int kBoxInteriorWidth = kBoxWidth - (kLineThinkness * 2);
const int kBoxInteriorHeight = kBoxHeight - (kLineThinkness * 2);

// Panel
const int kPanelStartX = kX;
const int kPanelStartY = kY;
const int kPanelWidth = kBoxWidth;
const int kPanelHeight = kMatrixHeight + (kMinoHeight * 2);

const Font kTextFont(Font::Typeface::Cabin, Font::Emphasis::Bold, 15);
} // namespace

void PlayerStatistic::Render(int y_offset) {
  int x = kBoxX;
  int y = kBoxY + y_offset;

  Pane::SetDrawColor(renderer_, Color::White);
  Pane::FillRect(renderer_, x, y, kBoxWidth, kBoxHeight);
  Pane::SetDrawColor(renderer_, Color::Black);
  Pane::FillRect(renderer_, x + kLineThinkness, y + kLineThinkness, kBoxInteriorWidth, kBoxInteriorHeight);

  name_ = "012345678901234  Game Over";
  auto [texture, w, h] = CreateTextureFromText(renderer_, assets_->GetFont(kTextFont), name_, Color::Yellow);

  Pane::RenderCopy(renderer_, texture.get(), x + kLineThinkness + 2, y + kLineThinkness + 2, w, h);

  int prev_h = h;
  name_ = "Score 999999  Level 99";
  std::tie(texture, w, h) = CreateTextureFromText(renderer_, assets_->GetFont(kTextFont), name_, Color::Yellow);

  Pane::RenderCopy(renderer_, texture.get(), x + kLineThinkness + 2, y + kLineThinkness + 2 + prev_h, w, h);

  prev_h += h;
  name_ = "Lines 9999 Sent 9999";
  std::tie(texture, w, h) = CreateTextureFromText(renderer_, assets_->GetFont(kTextFont), name_, Color::Yellow);

  Pane::RenderCopy(renderer_, texture.get(), x + kLineThinkness + 2, y + kLineThinkness + 2 + prev_h, w, h);
}

MultiPlayerPanel::MultiPlayerPanel(SDL_Renderer* renderer, Events& events, const std::shared_ptr<Assets>& assets)
      : Pane(renderer, kX, kY, assets), events_(events) {
  our_name_ = GetHostName();
}

void MultiPlayerPanel::Update(const Event& event) {
  switch (event.type()) {
    case Event::Type::PerfectClear:
      break;
    case Event::Type::SendLines:
      break;
    case Event::Type::GameOver:
      break;
    default:
      break;
  }
}

void MultiPlayerPanel::Render(double) {
  if (multiplayer_controller_) {
    multiplayer_controller_->Dispatch();
  }
  Pane::SetDrawColor(renderer_, Color::Black);
  Pane::FillRect(renderer_, kPanelStartX, kPanelStartY, kPanelWidth, kPanelHeight);
  PlayerStatistic stat(renderer_, "Turing", assets_);

  stat.Render(0.0);
}

void MultiPlayerPanel::Join(const std::string& name) {
  players_.insert(std::make_pair(name, std::make_shared<PlayerStatistic>(renderer_, name, assets_)));
}

void MultiPlayerPanel::Leave(const std::string& name) {
  players_.erase(name);
}

void MultiPlayerPanel::ResetCounter() { events_.Push(Event::Type::MultiPlayerResetCounter); }

void MultiPlayerPanel::StartGame(const std::string& name) {
  auto& stat = players_.at(name);

  stat->SetState(GameState::Playing);
  events_.Push(Event::Type::MultiPlayerStartGame);
}

void MultiPlayerPanel::Update(const std::string& name, size_t lines, size_t score, size_t level, GameState state) {
  auto& stat = players_.at(name);

  stat->Update(lines, score, level, state);

  std::sort(score_board_.begin(), score_board_.end(), [](const auto& a, const auto& b) { return a->score() < b->score(); });
}

void MultiPlayerPanel::GotLines(const std::string& name, size_t lines) {
  auto& stat = players_.at(name);

  stat->Update(lines);

  if (name != our_name_ ) {
    auto event = Event(Event::Type::MultiPlayerGotLines);

    event.SetGarbageLines(lines);
    events_.Push(event);
  }
}
