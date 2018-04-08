#include "game/panes/multi_player.h"

using namespace network;

namespace {

const int kX = kMatrixEndX + kMinoWidth + (kSpace * 4) + TextPane::kBoxWidth;
const int kY = kMatrixStartY - kMinoHeight;

const int kLineThinkness = 2;
const int kBoxWidth = kMultiPlayerPaneWidth;
const int kBoxHeight = 68;
const int kSpaceBetweenBoxes = 11;

const SDL_Rect kNameFieldRc = { kX, kY, 150, 24 };
const SDL_Rect kStateFieldRc = { kX + 148, kY, 72, 24 };
const SDL_Rect kScoreCaptionFieldRc = { kX, kY + 22, 220, 24 };
const SDL_Rect kScoreFieldRc = { kX + 50, kY + 22, 220, 24 };
const SDL_Rect kLinesCaptionFieldRc = { kX, kY + 44, 111, 24 };
const SDL_Rect kLinesFieldRc = { kX + 50, kY + 44, 111, 24 };
const SDL_Rect kLevelCaptionFieldRc = {kX + 109, kY + 44, 111, 24 };
const SDL_Rect kLevelFieldRc = { kX + 159, kY + 44, 111, 24 };

const Font kTextFont(Font::Typeface::Cabin, Font::Emphasis::Bold, 15);

using TextureID = PlayerData::TextureID;

struct Field {
  Field(TextureID id, const std::string& name, const SDL_Rect& rc, Color color = Color::SteelGray)
      : id_(id), name_(name), rc_(rc), color_(color){};
  TextureID id_;
  std::string name_;
  SDL_Rect rc_;
  Color color_;
};

 const std::vector<Field> kFields = {
  Field(TextureID::State, ToString(GameState::Idle), kStateFieldRc, Color::Yellow),
  Field(TextureID::ScoreCaption, "Score", kScoreCaptionFieldRc),
  Field(TextureID::Score, "0", kScoreFieldRc, Color::Yellow),
  Field(TextureID::LevelCaption, "Level", kLevelCaptionFieldRc),
  Field(TextureID::Level, "1", kLevelFieldRc, Color::Yellow),
  Field(TextureID::LinesCaption, "Lines", kLinesCaptionFieldRc),
  Field(TextureID::Lines, "0", kLinesFieldRc, Color::Yellow)
};

inline SDL_Rect* AddBorder(SDL_Rect& tmp, const SDL_Rect& rc) {
  tmp = { rc.x + kLineThinkness, rc.y + kLineThinkness, rc.w - (kLineThinkness * 2), rc.h - (kLineThinkness * 2) };
  return &tmp;
}

inline SDL_Rect* InsideBox(SDL_Rect& tmp, const SDL_Rect& rc, int w, int h) {
  tmp = { rc.x + (kLineThinkness * 2), rc.y  + kLineThinkness, w, h};
  return &tmp;
}

inline const SDL_Rect& AddYOffset(SDL_Rect& tmp, int offset, const SDL_Rect& rc) {
  tmp = { rc.x, rc.y + offset, rc.w, rc.h };
  return tmp;
}

} // namespace

PlayerData::PlayerData(SDL_Renderer* renderer, const std::string name, const std::shared_ptr<Assets>& assets)
      : renderer_(renderer), name_(name), assets_(assets) {
  auto [texture, w, h] = CreateTextureFromText(renderer_, assets_->GetFont(kTextFont), name, Color::Yellow);

  textures_.insert(std::make_pair(TextureID::Name, std::make_shared<Texture>(std::move(texture), w, h, kNameFieldRc)));
  for (const auto& field : kFields) {
    auto [texture, w, h] = CreateTextureFromText(renderer_, assets_->GetFont(kTextFont), field.name_, field.color_);

    textures_.insert(std::make_pair(field.id_, std::make_shared<Texture>(std::move(texture), w, h, field.rc_)));
  }
}

bool PlayerData::Update(int lines, int score, int level, network::GameState state) {
  auto resort_score_board = false;

  if (lines != 0 && lines_ != lines) {
    auto& stat = textures_.at(TextureID::Lines);

    auto [texture, w, h] = CreateTextureFromText(renderer_, assets_->GetFont(kTextFont), std::to_string(lines), Color::Yellow);

    stat->Set(std::move(texture), w, h);
    lines_ = lines;
  }
  if (score != 0 && score_ != score) {
    auto& stat = textures_.at(TextureID::Score);

    auto [texture, w, h] = CreateTextureFromText(renderer_, assets_->GetFont(kTextFont), std::to_string(score), Color::Yellow);
    stat->Set(std::move(texture), w, h);
    score_ = score;
    resort_score_board = true;
  }
  if (level != 0 && level_ != level) {
    auto& stat = textures_.at(TextureID::Level);

    auto [texture, w, h] = CreateTextureFromText(renderer_, assets_->GetFont(kTextFont), std::to_string(level), Color::Yellow);
    stat->Set(std::move(texture), w, h);
    level_ = level;
  }
  if (state != GameState::None && state_ != state) {
    auto& stat = textures_.at(TextureID::State);

    auto [texture, w, h] = CreateTextureFromText(renderer_, assets_->GetFont(kTextFont), ToString(state), Color::Yellow);
    stat->Set(std::move(texture), w, h);
    state_ = state;
  }

  return resort_score_board;
}

void PlayerData::Render(int offset,  bool is_my_status) const {
  Pane::SetDrawColor(renderer_, (is_my_status) ? Color::Green : Color::White);
  Pane::FillRect(renderer_, kX, kY + offset, kBoxWidth, kBoxHeight);
  Pane::SetDrawColor(renderer_, Color::Black);
  SDL_Rect tmp;

  SDL_RenderFillRect(renderer_, AddBorder(tmp, AddYOffset(tmp, offset, kNameFieldRc)));
  SDL_RenderFillRect(renderer_, AddBorder(tmp, AddYOffset(tmp, offset, kStateFieldRc)));
  SDL_RenderFillRect(renderer_, AddBorder(tmp, AddYOffset(tmp, offset, kScoreCaptionFieldRc)));
  SDL_RenderFillRect(renderer_, AddBorder(tmp, AddYOffset(tmp, offset, kLevelCaptionFieldRc)));
  SDL_RenderFillRect(renderer_, AddBorder(tmp, AddYOffset(tmp, offset, kLinesCaptionFieldRc)));
  for (const auto& it : textures_) {
    const auto& t = it.second;

    SDL_RenderCopy(renderer_, t->texture_.get(), nullptr, &AddYOffset(tmp, offset, *InsideBox(tmp, t->rc_, t->w_, t->h_)));
  }
}

MultiPlayer::MultiPlayer(SDL_Renderer* renderer, Events& events, const std::shared_ptr<Assets>& assets)
      : Pane(renderer, kX, kY, assets), events_(events) {}

void MultiPlayer::Update(const Event& event) {
  if (!multiplayer_controller_) {
    return;
  }
  switch (event.type()) {
    case Event::Type::CalculatedScore:
      progress_accumulator_.AddScore(event.score_);
      break;
    case Event::Type::ScoringData:
      progress_accumulator_.AddScore(event.lines_dropped_);
      progress_accumulator_.AddLines(event.lines_cleared());
      break;
    case Event::Type::LevelUp:
      progress_accumulator_.SetLevel(event.current_level_);
      break;
    case Event::Type::SendLines:
      multiplayer_controller_->SendUpdate(0, 0, 0, event.garbage_lines_);
      break;
    case Event::Type::GameOver:
      multiplayer_controller_->SendUpdate(0, 0, 0, GameState::GameOver);
      break;
    default:
      break;
  }
}

void MultiPlayer::Render(double delta_time) {
  if (multiplayer_controller_) {
    multiplayer_controller_->Dispatch();
  }
  Pane::SetDrawColor(renderer_, Color::Black);
  Pane::FillRect(renderer_, kX, kY, kMultiPlayerPaneWidth, kMultiPlayerPaneHeight);

  int offset = 0;

  for (const auto& player : score_board_) {
    player->Render((kBoxHeight + kSpaceBetweenBoxes) * offset, multiplayer_controller_->our_host_name() == player->name());
    offset++;
  }
  ticks_ += delta_time;
  if (ticks_ > 1.0) {
    ticks_ = 0.0;
    if (multiplayer_controller_ && progress_accumulator_.is_dirty_) {
      multiplayer_controller_->SendUpdate(progress_accumulator_.lines_, progress_accumulator_.score_,
                                          progress_accumulator_.level_, GameState::None);
      progress_accumulator_.is_dirty_  = false;
    }
  }
}

// ListenerInteface

void MultiPlayer::Join(const std::string& name) {
  score_board_.push_back(players_.insert(std::make_pair(name, std::make_shared<PlayerData>(renderer_, name, assets_))).first->second);
}

void MultiPlayer::Leave(const std::string& name) {
  auto it = std::find_if(score_board_.begin(), score_board_.end(), [&name](const auto& e) { return name == e->name(); });

  score_board_.erase(it);
  players_.erase(name);
}

void MultiPlayer::ResetCountDown() { events_.Push(Event::Type::MultiPlayerResetCounter); }

void MultiPlayer::StartGame(const std::string& name) {
  auto& stat = players_.at(name);

  stat->SetState(GameState::Playing);
  events_.Push(Event::Type::MultiPlayerStartGame);
}

void MultiPlayer::Update(const std::string& name, size_t lines, size_t score, size_t level, GameState state) {
  auto& stat = players_.at(name);

  if (stat->Update(lines, score, level, state)) {
    std::sort(score_board_.begin(), score_board_.end(), [](const auto& a, const auto& b) { return a->score() < b->score(); });
  }
}

void MultiPlayer::GotLines(const std::string& name, size_t lines) {
  if (name != multiplayer_controller_->our_host_name()) {
    auto event = Event(Event::Type::MultiPlayerGotLines);

    event.garbage_lines_ = lines;
    events_.Push(event);
  }
}
