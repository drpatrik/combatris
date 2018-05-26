#include "game/panes/player.h"

using namespace network;

namespace {

const int kPlayerMinoWidth = 8;
const int kPlayerMinoHeight = 8;
const int kLineThinkness = 2;
const int kMatrixStartPosX = 4;
const int kMatrixStartPosY = 46;

const SDL_Rect kNameFieldRc = { kX, kY, 140, 24 };
const SDL_Rect kStateFieldRc = { kX + 138, kY, 82, 24 };
const SDL_Rect kScoreCaptionFieldRc = { kX, kY + 22, 140, 24 };
const SDL_Rect kScoreFieldRc = { kX + 50, kY + 22, 140, 24 };
const SDL_Rect kLevelCaptionFieldRc = { kX + 138, kY + 22, 82, 24 };
const SDL_Rect kLevelFieldRc = { kX + 165, kY + 22, 82, 24 };
const SDL_Rect kKOCaptionFieldRc = { kX + 102, kY + 44, 118, 24 };
const SDL_Rect kKOFieldRc = { kX + 102, kY + 66, 118, 24 };
const SDL_Rect kLinesSentCaptionFieldRc = { kX + 102, kY + 90, 118, 24 };
const SDL_Rect kLinesSentFieldRc = { kX + 102, kY + 112, 118, 24 };
const SDL_Rect kLinesCaptionFieldRc = { kX + 102, kY + 136, 118, 24 };
const SDL_Rect kLinesFieldRc = { kX + 102, kY + 158, 118, 24 };
const SDL_Rect kMatrixFieldRc = { kX + kMatrixStartPosX, kY + kMatrixStartPosY, 84 + kPlayerMinoWidth, 166 + kPlayerMinoHeight };

const Font kTextFont(Font::Typeface::Cabin, Font::Emphasis::Bold, 15);

const Player::MatrixType kEmptyMatrix {
  {
    {kBorderID, kBorderID, kBorderID, kBorderID, kBorderID, kBorderID, kBorderID, kBorderID, kBorderID, kBorderID, kBorderID, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID},
    {kBorderID, kBorderID, kBorderID, kBorderID, kBorderID, kBorderID, kBorderID, kBorderID, kBorderID, kBorderID, kBorderID, kBorderID}
  }
};

using ID = Player::TextureID;

struct Field {
  Field(ID id, const std::string& name, const SDL_Rect& rc, Color color = Color::SteelGray)
      : id_(id), name_(name), rc_(rc), color_(color) {};
  ID id_;
  std::string name_;
  SDL_Rect rc_;
  Color color_;
};

const std::vector<Field> kFields = {
  Field(ID::State, ToString(GameState::Idle), kStateFieldRc, Color::Yellow),
  Field(ID::ScoreCaption, "Score", kScoreCaptionFieldRc),
  Field(ID::Score, "0", kScoreFieldRc, Color::Yellow),
  Field(ID::LevelCaption, "Lvl", kLevelCaptionFieldRc),
  Field(ID::Level, "1", kLevelFieldRc, Color::Yellow),
  Field(ID::KO, "0", kKOFieldRc, Color::Yellow),
  Field(ID::KOCaption, "KO's", kKOCaptionFieldRc),
  Field(ID::LinesSentCaption, "Lines Sent", kLinesSentCaptionFieldRc),
  Field(ID::LinesSent, "0", kLinesSentFieldRc, Color::Yellow),
  Field(ID::LinesCaption, "Lines Cleared", kLinesCaptionFieldRc),
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

inline const SDL_Rect& AddOffset(SDL_Rect& tmp, int x_offset, int y_offset, const SDL_Rect& rc) {
  tmp = { rc.x + x_offset, rc.y + y_offset, rc.w, rc.h };
  return tmp;
}

std::string IntToString(int v) { return std::to_string(v); }

} // namespace

Player::Player(SDL_Renderer* renderer, const std::string& name, uint64_t host_id, const std::shared_ptr<Assets>& assets)
    : renderer_(renderer), name_(name), host_id_(host_id), assets_(assets), tetrominos_(assets_->GetTetrominos()) {
  for (const auto& field : kFields) {
    auto [texture, w, h] = CreateTextureFromText(renderer_, assets_->GetFont(kTextFont), field.name_, field.color_);

    textures_.insert(std::make_pair(field.id_, std::make_shared<Texture>(std::move(texture), w, h, field.rc_)));
  }
  auto [texture, w, h] = CreateTextureFromText(renderer_, assets_->GetFont(kTextFont), name_, Color::Yellow);

  textures_.insert(std::make_pair(ID::Name, std::make_shared<Texture>(std::move(texture), w, h, kNameFieldRc)));
  matrix_ = kEmptyMatrix;
}

int Player::Update(Player::TextureID id, int new_value, int old_value, Function to_string, bool set_to_zero) {
  if (!set_to_zero && (new_value == 0 || new_value == old_value)) {
    return old_value;
  }
  auto& stat = textures_.at(id);

  auto [texture, w, h] = CreateTextureFromText(renderer_, assets_->GetFont(kTextFont), to_string(new_value), Color::Yellow);

  stat->Set(std::move(texture), w, h);

  return new_value;
}

void Player::ProgressUpdate(int lines, int score, int level, bool set_to_zero) {
  lines_ = Update(ID::Lines, lines, lines_, IntToString, set_to_zero);
  score_ = Update(ID::Score, score, score_, IntToString, set_to_zero);
  level_ = Update(ID::Level, level, level_, IntToString, set_to_zero);
}

void Player::SetMatrixState(const network::MatrixState& state) {
  int i = 0;

  for (size_t row = 1; row < matrix_.size() - 1; ++row) {
    auto& col_vec = matrix_[row];

    for (size_t col = 1; col < col_vec.size() - 1; col +=2) {
      col_vec[col] = state[i] >> 4;
      col_vec[col + 1] = state[i] & 0x0F;
      i++;
    }
  }
}

void Player::SetState(GameState state, bool set_to_zero) {
  state_ = static_cast<GameState>(Update(ID::State, static_cast<int>(state), static_cast<int>(state_),
                                         [](int v) { return ToString(static_cast<GameState>(v)); }, set_to_zero));
}

void Player::SetLinesSent(int lines_sent, bool set_to_zero) {
  lines_sent_ = Update(ID::LinesSent, lines_sent, lines_sent_, IntToString, set_to_zero);
}

void Player::SetKO(int ko, bool set_to_zero) { ko_ = Update(ID::KO, ko, ko_, IntToString, set_to_zero); }

void Player::Reset() {
  lines_ = 0;
  score_ = 0;
  level_ = 1;
  ProgressUpdate(lines_, score_, level_, true);
  lines_sent_ = 0;
  SetLinesSent(lines_sent_, true);
  ko_ = 0;
  SetKO(ko_, true);
  matrix_ = kEmptyMatrix;
}

void Player::Render(int x_offset, int y_offset, bool is_my_status) const {
  Pane::SetDrawColor(renderer_, (is_my_status) ? Color::Green : Color::White);
  Pane::FillRect(renderer_, kX + x_offset, kY + y_offset, kBoxWidth, kBoxHeight);
  Pane::SetDrawColor(renderer_, Color::Black);
  SDL_Rect tmp;

  SDL_RenderFillRect(renderer_, AddBorder(tmp, AddOffset(tmp, x_offset, y_offset, kNameFieldRc)));
  SDL_RenderFillRect(renderer_, AddBorder(tmp, AddOffset(tmp, x_offset, y_offset, kStateFieldRc)));
  SDL_RenderFillRect(renderer_, AddBorder(tmp, AddOffset(tmp, x_offset, y_offset, kScoreCaptionFieldRc)));
  SDL_RenderFillRect(renderer_, AddBorder(tmp, AddOffset(tmp, x_offset, y_offset, kLevelCaptionFieldRc)));
  SDL_RenderFillRect(renderer_, AddBorder(tmp, AddOffset(tmp, x_offset, y_offset, kMatrixFieldRc)));
  SDL_RenderFillRect(renderer_, AddBorder(tmp, AddOffset(tmp, x_offset, y_offset, kKOCaptionFieldRc)));
  SDL_RenderFillRect(renderer_, AddBorder(tmp, AddOffset(tmp, x_offset, y_offset, kKOFieldRc)));
  SDL_RenderFillRect(renderer_, AddBorder(tmp, AddOffset(tmp, x_offset, y_offset, kLinesSentCaptionFieldRc)));
  SDL_RenderFillRect(renderer_, AddBorder(tmp, AddOffset(tmp, x_offset, y_offset, kLinesSentFieldRc)));
  SDL_RenderFillRect(renderer_, AddBorder(tmp, AddOffset(tmp, x_offset, y_offset, kLinesCaptionFieldRc)));
  SDL_RenderFillRect(renderer_, AddBorder(tmp, AddOffset(tmp, x_offset, y_offset, kLinesFieldRc)));

  for (const auto& it : textures_) {
    SDL_RenderCopy(renderer_, it.second->texture_.get(), nullptr,
                   &AddOffset(tmp, x_offset, y_offset, *InsideBox(tmp, it.second->rc_, it.second->w_, it.second->h_)));
  }
  int y_pos = 0;

  for (size_t row = 0; row < matrix_.size(); ++row) {
    int x_pos = 0;

    for (size_t col = 0; col < matrix_[row].size(); ++col) {
      const auto id = matrix_[row][col];

      if (kEmptyID == id) {
        x_pos += kPlayerMinoWidth;
        continue;
      }
      const int x = kX + kMatrixStartPosX + x_pos + x_offset;
      const int y = kY + kMatrixStartPosY + y_pos + y_offset;

      RenderMino(renderer_, x, y, kPlayerMinoWidth, kPlayerMinoHeight, tetrominos_[id - 1]->texture());
      x_pos += kPlayerMinoWidth;
    }
    y_pos += kPlayerMinoHeight;
  }
}
