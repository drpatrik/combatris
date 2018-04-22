#pragma once

#include "game/events.h"
#include "game/panes/player.h"
#include "game/panes/pane.h"

#include <vector>
#include <unordered_map>

class MultiPlayer final : public Pane, public EventListener,  public network::ListenerInterface {
 public:
  MultiPlayer(SDL_Renderer* renderer, Events& events, const std::shared_ptr<Assets>& assets);

  virtual ~MultiPlayer() noexcept {}

  virtual void Update(const Event& event) override;

  virtual void Reset() override {};

  virtual void Render(double) override;

  void Enable() {
    multiplayer_controller_ = std::make_unique<network::MultiPlayerController>(this);
    multiplayer_controller_->Join();
  }

  void Disable() {
    multiplayer_controller_.reset();
    score_board_.clear();
    players_.clear();
  }

  bool CanPressNewGame() const {
    return std::none_of(score_board_.begin(), score_board_.end(), [](const auto& p) { return p->state() == network::GameState::Playing; });
  }

  void NewGame() { multiplayer_controller_->NewGame(); }

  void StartGame() { multiplayer_controller_->StartGame(); }

  const std::string& our_host_name() const { return multiplayer_controller_->our_host_name(); }

 protected:
  virtual bool GotJoin(const std::string& name) override;

  virtual void GotLeave(const std::string& name) override;

  virtual void GotNewGame(const std::string& name) override;

  virtual void GotStartGame() override;

  virtual void GotUpdate(const std::string& name, int lines, int lines_sent, int score, int level, network::GameState state) override;

  virtual void GotKnockedOutBy(uint64_t name_hash) override;

  virtual void GotLines(const std::string& name, int lines) override;

 private:
  struct GameStatisticsAccumlator {
    void AddLines(int lines) {
      lines_ += lines;
      is_dirty_ = (lines > 0);
    }

    void AddLinesSent(int lines) {
      lines_sent_ += lines;
      is_dirty_ = (lines > 0);
    }

    void AddScore(int score) {
      score_ += score;
      is_dirty_ = (score > 0);
    }

    void SetLevel(int level) {
      level_ = level;
      is_dirty_ = true;
    }

    void Reset() {
      lines_ = 0;
      lines_sent_ = 0;
      score_ = 0;
      level_ = 0;
      is_dirty_ = false;
    }

    int lines_ = 0;
    int lines_sent_;
    int score_ = 0;
    int level_ = 0;
    bool is_dirty_ = false;
  };

  Events& events_;
  network::GameState game_state_ = network::GameState::None;
  std::vector<Player::Ptr> score_board_;
  std::unordered_map<std::string, Player::Ptr> players_;
  std::unique_ptr<network::MultiPlayerController> multiplayer_controller_;
  GameStatisticsAccumlator accumulator_;
  double ticks_ = 0.0;
};
