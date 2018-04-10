#include "game/panes/player.h"

using namespace network;

namespace {

const int kLineThinkness = 2;

const SDL_Rect kNameFieldRc = { kX, kY, 140, 24 };
const SDL_Rect kStateFieldRc = { kX + 138, kY, 82, 24 };
const SDL_Rect kScoreCaptionFieldRc = { kX, kY + 22, 220, 24 };
const SDL_Rect kScoreFieldRc = { kX + 50, kY + 22, 220, 24 };
const SDL_Rect kLinesCaptionFieldRc = { kX, kY + 44, 111, 24 };
const SDL_Rect kLinesFieldRc = { kX + 50, kY + 44, 111, 24 };
const SDL_Rect kLevelCaptionFieldRc = {kX + 109, kY + 44, 111, 24 };
const SDL_Rect kLevelFieldRc = { kX + 159, kY + 44, 111, 24 };

const Font kTextFont(Font::Typeface::Cabin, Font::Emphasis::Bold, 15);

using ID = Player::TextureID;

struct Field {
  Field(ID id, const std::string& name, const SDL_Rect& rc, Color color = Color::SteelGray)
      : id_(id), name_(name), rc_(rc), color_(color){};
  ID id_;
  std::string name_;
  SDL_Rect rc_;
  Color color_;
};

 const std::vector<Field> kFields = {
  Field(ID::State, ToString(GameState::Idle), kStateFieldRc, Color::Yellow),
  Field(ID::ScoreCaption, "Score", kScoreCaptionFieldRc),
  Field(ID::Score, "0", kScoreFieldRc, Color::Yellow),
  Field(ID::LevelCaption, "Level", kLevelCaptionFieldRc),
  Field(ID::Level, "1", kLevelFieldRc, Color::Yellow),
  Field(ID::LinesCaption, "Lines", kLinesCaptionFieldRc),
  Field(ID::Lines, "0", kLinesFieldRc, Color::Yellow)
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

bool Player::Update(int lines, int score, int level, GameState state) {
  auto resort_score_board = false;

  if (lines != 0 && lines_ != lines) {
    auto& stat = textures_.at(ID::Lines);

    auto [texture, w, h] = CreateTextureFromText(renderer_, assets_->GetFont(kTextFont), std::to_string(lines), Color::Yellow);

    stat->Set(std::move(texture), w, h);
    lines_ = lines;
  }
  if (score != 0 && score_ != score) {
    auto& stat = textures_.at(ID::Score);

    auto [texture, w, h] = CreateTextureFromText(renderer_, assets_->GetFont(kTextFont), std::to_string(score), Color::Yellow);
    stat->Set(std::move(texture), w, h);
    score_ = score;
    resort_score_board = true;
  }
  if (level != 0 && level_ != level) {
    auto& stat = textures_.at(ID::Level);

    auto [texture, w, h] = CreateTextureFromText(renderer_, assets_->GetFont(kTextFont), std::to_string(level), Color::Yellow);
    stat->Set(std::move(texture), w, h);
    level_ = level;
  }
  if (state != GameState::None && state_ != state) {
    auto& stat = textures_.at(ID::State);

    auto [texture, w, h] = CreateTextureFromText(renderer_, assets_->GetFont(kTextFont), ToString(state), Color::Yellow);
    stat->Set(std::move(texture), w, h);
    state_ = state;
  }

  return resort_score_board;
}

void Player::Reset(bool force_reset) {
  if (!force_reset && network::GameState::None == state_) {
    return;
  }
  textures_.clear();
  for (const auto& field : kFields) {
    auto [texture, w, h] = CreateTextureFromText(renderer_, assets_->GetFont(kTextFont), field.name_, field.color_);

    textures_.insert(std::make_pair(field.id_, std::make_shared<Texture>(std::move(texture), w, h, field.rc_)));
  }
	auto[texture, w, h] = CreateTextureFromText(renderer_, assets_->GetFont(kTextFont), name_, Color::Yellow);

	textures_.insert(std::make_pair(ID::Name, std::make_shared<Texture>(std::move(texture), w, h, kNameFieldRc)));
}

void Player::Render(int offset,  bool is_my_status) const {
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
