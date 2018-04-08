#pragma once

#include "game/events.h"
#include "game/panes/pane.h"
#include "network/multiplayer_controller.h"

#include <vector>
#include <unordered_map>

class PlayerData {
 public:
  enum TextureID { Name, State, ScoreCaption, Score, LevelCaption, Level, LinesCaption, Lines };

  PlayerData(SDL_Renderer* renderer, const std::string name, const std::shared_ptr<Assets>& assets);

  bool Update(int lines, int score, int level, network::GameState state);

  void Reset() {}

  void Render(int y_offset, bool is_my_status) const;

  void SetState(network::GameState state) { state_ = state; }

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
  network::GameState state_ = network::GameState::Waiting;
  std::unordered_map<TextureID, std::shared_ptr<Texture>> textures_;
};

class MultiPlayer final : public Pane, public EventSink,  public network::ListenerInterface {
 public:
  MultiPlayer(SDL_Renderer* renderer, Events& events, const std::shared_ptr<Assets>& assets);

  virtual void Update(const Event& event) override;

  virtual void Reset() override { progress_accumulator_.Reset(); }

  virtual void Render(double) override;

  void Enable() {
    multiplayer_controller_ = std::make_unique<network::MultiPlayerController>(this);
    multiplayer_controller_->Join();
  }

  void Disable() {
    Leave(our_name_);
    multiplayer_controller_->Leave();
    multiplayer_controller_.reset();
  }

  void ResetCountDown() {
    if (!multiplayer_controller_) {
      return;
    }
    multiplayer_controller_->ResetCounter();
  }

 protected:
  virtual void Join(const std::string& name) override;

  virtual void Leave(const std::string& name) override;

  virtual void ResetCounter() override;

  virtual void StartGame(const std::string& name) override;

  virtual void Update(const std::string& name, size_t lines, size_t score, size_t level, network::GameState state) override;

  virtual void GotLines(const std::string& name, size_t lines) override;

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
  std::string our_name_;
  ProgressAccumlator progress_accumulator_;
  double ticks_ = 0.0;
};
