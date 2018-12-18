#pragma once

#include <assert.h>

#include "game/matrix.h"
#include "game/panes/accumlator.h"
#include "game/panes/player.h"
#include "game/panes/pane.h"

class MultiPlayer final : public Pane, public EventListener,  public network::ListenerInterface {
 public:
  MultiPlayer(SDL_Renderer* renderer, const std::shared_ptr<Matrix>& matrix, Events& events, const std::shared_ptr<Assets>& assets);

  virtual ~MultiPlayer() noexcept {}

  virtual void Update(const Event& event) override;

  virtual void Reset() override {}

  virtual void Render(double) override;

  void Enable() {
    if (multiplayer_controller_ != nullptr) {
      return;
    }
    multiplayer_controller_ = std::make_unique<network::MultiPlayerController>(this);
    multiplayer_controller_->Join();
  }

  void Disable() {
    if (!multiplayer_controller_) {
      return;
    }
    multiplayer_controller_.reset();
    score_board_.clear();
    players_.clear();
  }

  inline bool CanPressNewGame() const {
    return std::none_of(score_board_.begin(), score_board_.end(), [](const auto& p) { return p->state() == network::GameState::Playing; });
  }

  inline void NewGame() { multiplayer_controller_->NewGame(campaign_type_); }

  inline const std::string& our_host_name() const {
    assert(multiplayer_controller_ != nullptr);
    return multiplayer_controller_->our_host_name();
  }

 protected:
  virtual bool GotJoin(const std::string& name, uint64_t host_id) override;

  virtual void GotLeave(uint64_t host_id) override;

  virtual void GotNewGame(uint64_t host_id, CampaignType type) override;

  virtual void GotStartGame() override;

  virtual void GotNewState(uint64_t host_id, network::GameState state) override;

  virtual void GotProgressUpdate(uint64_t host_id, int lines, int score, int level, const network::MatrixState&) override;

  virtual void GotLines(uint64_t host_id, int lines) override;

  virtual void GotPlayerKnockedOut(uint64_t host_id) override;

  virtual void GotTime(uint64_t host_id, uint64_t time) override;

private:
  inline bool IsUs(uint64_t host_id) const { return multiplayer_controller_->IsUs(host_id); }

  void SortScoreBoard();

  std::shared_ptr<Matrix> matrix_;
  Events& events_;
  network::GameState game_state_ = network::GameState::None;
  std::vector<Player::Ptr> score_board_;
  std::unordered_map<uint64_t, Player::Ptr> players_;
  std::unique_ptr<network::MultiPlayerController> multiplayer_controller_;
  Accumlator accumulator_;
  double ticks_progess_update_ = 0.0;
  CampaignType campaign_type_ = CampaignType::Combatris;
  int start_level_ = 1;
};
