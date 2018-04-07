#pragma once

#include "game/events.h"
#include "game/panes/pane.h"
#include "network/multiplayer_controller.h"

#include <vector>
#include <unordered_map>

class PlayerStatistic {
 public:
  PlayerStatistic(SDL_Renderer* renderer, const std::string name, const std::shared_ptr<Assets>& assets)
      : renderer_(renderer), name_(name), assets_(assets) {}

  void Update(size_t lines, size_t score, size_t level, network::GameState state) {
    lines_ = lines;
    score_ = score;
    level_ = level;
    state_ = state;
    is_dirty_ = true;
  }

  void Update(int garbage_sent) { garbage_sent_ += garbage_sent; }

  void Reset() { garbage_sent_ = 0; }

  void Render(int y_offset);

  void SetState(network::GameState state) { state_ = state; }

  int score() const { return score_; }

 private:
  SDL_Renderer* renderer_;
  std::string name_;
  const std::shared_ptr<Assets>& assets_;
  int lines_ = 0;
  int score_ = 0;
  int level_ = 0;
  int garbage_sent_ = 0;
  network::GameState state_ = network::GameState::Waiting;
  bool is_dirty_ = true;
};

class MultiPlayerPanel final : public Pane, public EventSink,  public network::ListenerInterface {
 public:
  MultiPlayerPanel(SDL_Renderer* renderer, Events& events, const std::shared_ptr<Assets>& assets);

  virtual void Update(const Event& event) override;

  virtual void Reset() override {}

  virtual void Render(double) override;

  void Enable() {
    multiplayer_controller_ = std::make_unique<network::MultiPlayerController>(this);
    multiplayer_controller_->Join();
  }

  void Disable() {
    multiplayer_controller_->Leave();
    multiplayer_controller_.reset();
  }

 protected:
  virtual void Join(const std::string& name) override;

  virtual void Leave(const std::string& name) override;

  virtual void ResetCounter() override;

  virtual void StartGame(const std::string& name) override;

  virtual void Update(const std::string& name, size_t lines, size_t score, size_t level, network::GameState state) override;

  virtual void GotLines(const std::string& name, size_t lines) override;

 private:
  using PlayerStatisticPtr = std::shared_ptr<PlayerStatistic>;

  Events& events_;
  std::vector<PlayerStatisticPtr> score_board_;
  std::unordered_map<std::string, PlayerStatisticPtr> players_;
  std::unique_ptr<network::MultiPlayerController> multiplayer_controller_;
  std::string our_name_;
};
