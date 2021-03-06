#pragma once

#include "game/panes/pane.h"
#include "network/multiplayer_controller.h"

#include <functional>

namespace {

const int kX = kMatrixEndX + kMinoWidth + (kSpace * 4) + TextPane::kBoxWidth;
const int kY = kMatrixStartY - kMinoHeight;

} // namespace

class Player final {
 public:
  enum TextureID {
    Name,
    State,
    ScoreCaption,
    Score,
    KOCaption,
    KO,
    LevelCaption,
    Level,
    LinesCaption,
    Lines,
    LinesSentCaption,
    LinesSent,
    TimeCaption,
    Time,
    Campaign,
    LastEntry
  };
  using Ptr = std::shared_ptr<Player>;
  using GameState = network::GameState;
  using CampaignType = network::CampaignType;
  using Function = std::function<std::string(int)>;
  using MatrixType = std::array<std::array<uint8_t, kVisibleCols + 2>, kVisibleRows + 2>;

  Player(SDL_Renderer* renderer, const std::string& name, uint64_t host_id, bool is_us, const std::shared_ptr<Assets>& assets);

  Player(const Player&) = delete;

  Player(const Player&&) noexcept = delete;

  inline const std::string& name() const { return name_; }

  inline uint64_t host_id() const { return host_id_; }

  bool ProgressUpdate(int lines, int score, int level, bool set_to_zero = false);

  inline int lines() const { return lines_; }

  inline int score() const { return score_; }

  void SetMatrixState(const network::MatrixState& state);

  void SetState(GameState state, bool set_to_zero = false);

  inline GameState state() const { return state_; }

  void AddLinesSent(int lines_sent, bool set_to_zero = false);

  inline int lines_sent() const { return lines_sent_; }

  void AddKO(int ko, bool set_to_zero = false);

  inline int ko() const { return ko_; }

  void SetTime(uint64_t time);

  inline uint64_t time() const { return time_; }

  void SetCampaignType(CampaignType type);

  inline bool IsSameCampaignType(CampaignType type) const {
    return GameState::Idle == state_ || GameState::GameOver == state_ || campaign_type_ == type;
  }

  void Reset();

  void Render(int x_offset, int y_offset) const;

 private:
  struct Field {
    std::unique_ptr<utility::Texture> texture_;
    SDL_Rect rc_;
  };

  int Update(Player::TextureID id, int new_value, int old_value, Function to_string, bool set_to_zero = false);

  SDL_Renderer* renderer_;
  std::string name_;
  uint64_t host_id_;
  bool is_us_ = false;
  const std::shared_ptr<Assets>& assets_;
  int lines_ = 0;
  int lines_sent_ = 0;
  int score_ = 0;
  int level_ = -1;
  int ko_ = 0;
  uint64_t time_ = 0;
  GameState state_ = GameState::None;
  MatrixType matrix_;
  CampaignType campaign_type_ = CampaignType::None;
  std::vector<std::shared_ptr<const Tetromino>> tetrominos_;
  std::array<Field, TextureID::LastEntry> fields_;
};
