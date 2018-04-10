#pragma once

#include "game/events.h"
#include "game/panes/player.h"
#include "game/panes/pane.h"

#include <vector>
#include <unordered_map>

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

  network::GameState state() const { return game_state_; }

  void NewGame() { multiplayer_controller_->NewGame(); }

  void StartGame() { multiplayer_controller_->StartGame(); }

  void ResetCountDown() { multiplayer_controller_->ResetCountDown(); }

 protected:
  virtual void GotJoin(const std::string& name) override;

  virtual void GotLeave(const std::string& name) override;

  virtual void GotResetCountDown(const std::string& name) override;

  virtual void GotStartGame() override;

  virtual void GotUpdate(const std::string& name, size_t lines, size_t score, size_t level, network::GameState state) override;

  virtual void GotLines(const std::string& name, size_t lines) override;

 private:
  struct GameStatisticsAccumlator {
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

  Events& events_;
  network::GameState game_state_ = network::GameState::None;
  std::vector<Player::Ptr> score_board_;
  std::unordered_map<std::string, Player::Ptr> players_;
  std::unique_ptr<network::MultiPlayerController> multiplayer_controller_;
  GameStatisticsAccumlator accumulator_;
  double ticks_ = 0.0;
};
