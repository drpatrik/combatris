#pragma once

#include "game/events.h"
#include "game/panes/pane.h"
#include "network/multiplayer_controller.h"

#include <vector>
#include <unordered_map>

class PlayerData {
 public:
  enum TextureID { Name, State, ScoreCaption, Score, LevelCaption, Level, LinesCaption, Lines };

  PlayerData(SDL_Renderer* renderer, const std::string name, const std::shared_ptr<Assets>& assets)
      : renderer_(renderer), name_(name), assets_(assets) { Reset(true); }

  bool Update(int lines, int score, int level, network::GameState state);

  void Reset(bool force_reset = false);

  void Render(int y_offset, bool is_my_status) const;

  const std::string& name() const { return name_; }

  int score() const { return score_; }

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

class MultiPlayer final : public Pane, public EventSink,  public network::ListenerInterface {
 public:
  MultiPlayer(SDL_Renderer* renderer, Events& events, const std::shared_ptr<Assets>& assets);

  virtual ~MultiPlayer() noexcept {
    if (multiplayer_controller_) {
      Disable();
    }
  }

  virtual void Update(const Event& event) override;

  virtual void Reset() override {
    for (auto& player : score_board_) {
      player->Reset();
    }
  }

  virtual void Render(double) override;

  void Enable() {
    multiplayer_controller_ = std::make_unique<network::MultiPlayerController>(this);
    multiplayer_controller_->Join();
  }

  void Disable() {
    GotLeave(multiplayer_controller_->our_host_name());
    multiplayer_controller_->Leave();
    multiplayer_controller_.reset();
  }

  void NewGame() { multiplayer_controller_->NewGame(); }

  void StartGame() { multiplayer_controller_->StartGame(); }

  void ResetCountDown() { multiplayer_controller_->ResetCountDown(); }

 protected:
  virtual void GotJoin(const std::string& name) override;

  virtual void GotLeave(const std::string& name) override;

  virtual void GotResetCountDown() override;

  virtual void GotStartGame() override;

  virtual void GotUpdate(const std::string& name, size_t lines, size_t score, size_t level, network::GameState state) override;

  virtual void GotGarbage(const std::string& name, size_t lines) override;

 private:
  struct ProgressAccumlator {
    void AddLines(int lines) { lines_ += lines; is_dirty_ = true; }

    void AddScore(int score) { score_ += score; is_dirty_ = true; }

    void SetLevel(int level) { level_ = level; is_dirty_ = true; }

    int lines_ = 0;
    int score_ = 0;
    int level_ = 0;
    bool is_dirty_ = false;

    void Reset() {
      lines_ = 0;
      score_ = 0;
      level_ = 0;
      is_dirty_ = false;
    }
  };
  using PlayerDataPtr = std::shared_ptr<PlayerData>;

  Events& events_;
  std::vector<PlayerDataPtr> score_board_;
  std::unordered_map<std::string, PlayerDataPtr> players_;
  std::unique_ptr<network::MultiPlayerController> multiplayer_controller_;
  ProgressAccumlator progress_accumulator_;
  double ticks_ = 0.0;
};
