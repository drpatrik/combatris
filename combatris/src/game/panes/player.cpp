#include "game/panes/player.h"

using namespace utility;
using namespace network;

namespace {

const int kPlayerMinoWidth = 8;
const int kPlayerMinoHeight = 8;
const int kLineThinkness = 2;
const int kMatrixStartPosX = 2;
const int kMatrixStartPosY = 46;

const SDL_Rect kNameFieldRc = { kX, kY, 140, 24 };
const SDL_Rect kStateFieldRc = { kX + 138, kY, 82, 24 };
const SDL_Rect kScoreCaptionFieldRc = { kX, kY + 22, 140, 24 };
const SDL_Rect kScoreFieldRc = { kX + 50, kY + 22, 140, 24 };
const SDL_Rect kLevelCaptionFieldRc = { kX + 138, kY + 22, 82, 24 };
const SDL_Rect kLevelFieldRc = { kX + 165, kY + 22, 82, 24 };
const SDL_Rect kKOCaptionFieldRc = { kX + 98, kY + 44, 122, 24 };
const SDL_Rect kKOFieldRc = { kX + 98, kY + 66, 122, 24 };
const SDL_Rect kLinesSentCaptionFieldRc = { kX + 98, kY + 90, 122, 24 };
const SDL_Rect kLinesSentFieldRc = { kX + 98, kY + 112, 122, 24 };
const SDL_Rect kLinesCaptionFieldRc = { kX + 98, kY + 136, 122, 24 };
const SDL_Rect kLinesFieldRc = { kX + 98, kY + 158, 122, 24 };
const SDL_Rect kTimeCaptionFieldRc = { kX + 98, kY + 182, 122, 24 };
const SDL_Rect kTimeFieldRc = { kX + 138, kY + 182, 82, 24 };
const SDL_Rect kCampaignFieldRc = { kX + 98, kY + 206, 122, 18 };
const SDL_Rect kMatrixFieldRc = { kX + kMatrixStartPosX, kY + kMatrixStartPosY, 84 + kPlayerMinoWidth, 166 + kPlayerMinoHeight };

const Font kTextFont(Font::Typeface::Cabin, Font::Emphasis::Bold, 15);
const Font kCampaignFont(Font::Typeface::Cabin, Font::Emphasis::Bold, 10);

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
  Field(ID::Level, "-", kLevelFieldRc, Color::Yellow),
  Field(ID::KO, "0", kKOFieldRc, Color::Yellow),
  Field(ID::KOCaption, "Knockouts", kKOCaptionFieldRc),
  Field(ID::LinesSentCaption, "Lines Sent", kLinesSentCaptionFieldRc),
  Field(ID::LinesSent, "0", kLinesSentFieldRc, Color::Yellow),
  Field(ID::LinesCaption, "Lines Cleared", kLinesCaptionFieldRc),
  Field(ID::Lines, "0", kLinesFieldRc, Color::Yellow),
  Field(ID::TimeCaption, "Time", kTimeCaptionFieldRc),
  Field(ID::Time, FormatTimeMMSSHS(0), kTimeFieldRc, Color::Yellow),
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

Player::Player(SDL_Renderer* renderer, const std::string& name, uint64_t host_id, bool is_us, const std::shared_ptr<Assets>& assets)
    : renderer_(renderer), name_(name), host_id_(host_id), is_us_(is_us), assets_(assets), tetrominos_(assets_->GetTetrominos()) {
  for (const auto& field : kFields) {
    fields_[field.id_].texture_ = std::make_unique<Texture>(renderer_, assets_->GetFont(kTextFont), field.name_, field.color_);
    fields_[field.id_].rc_ = field.rc_;
  }
  fields_[ID::Name].texture_ = std::make_unique<Texture>(renderer_, assets_->GetFont(kTextFont), name_, Color::Yellow);
  fields_[ID::Name].rc_ = kNameFieldRc;
  fields_[ID::Campaign].rc_ = kCampaignFieldRc;
  SetCampaignType(campaign_type_);
  matrix_ = kEmptyMatrix;
}

int Player::Update(Player::TextureID id, int new_value, int old_value, Function to_string, bool set_to_zero) {
  if (!set_to_zero && (0 == new_value || new_value == old_value)) {
    return old_value;
  }
  auto txt = (-1 == new_value) ? "-" : to_string(new_value);

  fields_[id].texture_ = std::make_unique<Texture>(renderer_, assets_->GetFont(kTextFont), txt, Color::Yellow);

  return new_value;
}

bool Player::ProgressUpdate(int lines, int score, int level, bool set_to_zero) {
  lines_ = Update(ID::Lines, lines, lines_, IntToString, set_to_zero);
  bool resort_score_board = (score != 0) && (score != score_);
  score_ = Update(ID::Score, score, score_, IntToString, set_to_zero);
  level_ = Update(ID::Level, level, level_, IntToString, set_to_zero);

  return resort_score_board;
}

void Player::SetMatrixState(const network::MatrixState& state) {
  int i = 0;

  for (int row = 1; row < static_cast<int>(matrix_.size() - 1); ++row) {
    auto& col_vec = matrix_[row];

    for (int col = 1; col < static_cast<int>(col_vec.size() - 1); col +=2) {
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

void Player::AddLinesSent(int lines_sent, bool set_to_zero) {
  lines_sent_ = Update(ID::LinesSent, lines_sent + lines_sent_, lines_sent_, IntToString, set_to_zero);
}

void Player::AddKO(int ko, bool set_to_zero) {
  ko_ = Update(ID::KO, ko + ko_, ko_, IntToString, set_to_zero);
}

void Player::SetTime(uint64_t time) {
  time_ = time;
  fields_[ID::Time].texture_ = std::make_unique<Texture>(renderer_, assets_->GetFont(kTextFont), FormatTimeMMSSHS(time_), Color::Yellow);
}

void Player::SetCampaignType(CampaignType type) {
  campaign_type_ = type;
  fields_[ID::Campaign].texture_ =
      std::make_unique<Texture>(renderer_, assets_->GetFont(kCampaignFont), ToString(campaign_type_), Color::Yellow);
}

void Player::Reset() {
  lines_ = 0;
  score_ = 0;
  level_ = -1;
  ProgressUpdate(lines_, score_, level_, true);
  lines_sent_ = 0;
  AddLinesSent(lines_sent_, true);
  ko_ = 0;
  AddKO(ko_, true);
  time_ = 0;
  SetTime(time_);
  matrix_ = kEmptyMatrix;
}

void Player::Render(int x_offset, int y_offset) const {
  Pane::SetDrawColor(renderer_, is_us_ ? Color::Green : Color::White);
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
  SDL_RenderFillRect(renderer_, AddBorder(tmp, AddOffset(tmp, x_offset, y_offset, kTimeCaptionFieldRc)));
  SDL_RenderFillRect(renderer_, AddBorder(tmp, AddOffset(tmp, x_offset, y_offset, kCampaignFieldRc)));

  for (const auto& field : fields_) {
    SDL_RenderCopy(renderer_, *field.texture_, nullptr,
                   &AddOffset(tmp, x_offset, y_offset,
                              *InsideBox(tmp, field.rc_, field.texture_->width(), field.texture_->height())));
  }
  int y_pos = 0;

  for (int row = 0; row < static_cast<int>(matrix_.size()); ++row) {
    int x_pos = 0;

    for (int col = 0; col < static_cast<int>(matrix_[row].size()); ++col) {
      const auto id = matrix_[row][col];

      if (kEmptyID == id) {
        x_pos += kPlayerMinoWidth;
        continue;
      }
      RenderMino(renderer_, kX + kMatrixStartPosX + x_pos + x_offset, kY + kMatrixStartPosY + y_pos + y_offset,
                 kPlayerMinoWidth, kPlayerMinoHeight, tetrominos_[id - 1]->texture());
      x_pos += kPlayerMinoWidth;
    }
    y_pos += kPlayerMinoHeight;
  }
}
