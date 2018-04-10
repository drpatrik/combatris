#pragma once

#include "game/panes/pane.h"
#include "network/multiplayer_controller.h"

namespace {

const int kX = kMatrixEndX + kMinoWidth + (kSpace * 4) + TextPane::kBoxWidth;
const int kY = kMatrixStartY - kMinoHeight;
const int kBoxWidth = kMultiPlayerPaneWidth;
const int kBoxHeight = 68;

} // namesapce

class Player final {
 public:
  enum TextureID { Name, State, ScoreCaption, Score, LevelCaption, Level, LinesCaption, Lines };
  using Ptr = std::shared_ptr<Player>;

  Player(SDL_Renderer* renderer, const std::string name, const std::shared_ptr<Assets>& assets,
         network::GameState state = network::GameState::None)
      : renderer_(renderer), name_(name), assets_(assets), state_(state) {
    Reset(true);
  }

  Player(const Player&) = delete;

  bool Update(int lines, int score, int level, network::GameState state);

  void Reset(bool force_reset = false);

  void Render(int y_offset, bool is_my_status) const;

  const std::string& name() const { return name_; }

  int score() const { return score_; }

  network::GameState state() const { return state_; }

  void SetState(network::GameState state) { state_ = state; }

 private:
  struct Texture {
    Texture(UniqueTexturePtr&& texture, int w, int h, SDL_Rect rc)
        : texture_(std::move(texture)), w_(w), h_(h), rc_(rc) {}

    void Set(UniqueTexturePtr&& texture, int w, int h) {
      texture_ = std::move(texture);
      w_ = w;
      h_ = h;
    }
    UniqueTexturePtr texture_ = nullptr;
    int w_;
    int h_;
    SDL_Rect rc_;
  };

  SDL_Renderer* renderer_;
  std::string name_;
  const std::shared_ptr<Assets>& assets_;
  int lines_ = 0;
  int score_ = 0;
  int level_ = 0;
  network::GameState state_ = network::GameState::None;
  std::unordered_map<TextureID, std::shared_ptr<Texture>> textures_;
};
