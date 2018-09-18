#pragma once

#include "game/panes/timer.h"
#include "game/panes/multi_player.h"
#include "game/panes/total_lines.h"
#include "game/panes/lines_sent.h"
#include "game/panes/goal.h"
#include "game/panes/scoring.h"
#include "game/panes/high_score.h"
#include "game/panes/knockout.h"
#include "game/panes/next_queue.h"
#include "game/panes/hold_queue.h"
#include "game/panes/moves.h"
#include "game/campaign_types.h"

class Campaign {
 public:
  Campaign(SDL_Renderer* renderer, Events& events, const std::shared_ptr<Assets>& assets, const std::shared_ptr<Matrix>& matrix);

  operator CampaignType() const { return type_; }

  void Set(SDL_Window* window, CampaignType type);

  void Render(double delta_time);

  Event PreprocessEvent(const Event& event);

  void Reset() {
    std::for_each(panes_.begin(), panes_.end(), [](const auto& r) { r->Reset(); });
    tetromino_generator_->Reset();
    next_queue_->Hide();
  }

  inline void ShowNextQueue() { next_queue_->Show(); }

  inline void Pause() { next_queue_->Hide(); }

  void Unpause() {
    next_queue_->Show();
    level_->ResetTime();
  }

  inline std::shared_ptr<MultiPlayer> GetMultiPlayerPane() { return multi_player_; }

  inline std::shared_ptr<Level> GetLevel() { return level_; }

  inline std::shared_ptr<HoldQueue> GetHoldQueuePane() { return hold_queue_; }

  inline std::shared_ptr<TetrominoGenerator> GetTetrominoGenerator() { return tetromino_generator_; }

 protected:
  void AddListener(EventListener* listener) { event_listeners_.push_back(listener); }

  void SetupCampaign(CampaignType type);

 private:
  SDL_Renderer* renderer_;
  Events& events_;
  std::shared_ptr<Assets> assets_;
  std::shared_ptr<TetrominoGenerator> tetromino_generator_;
  std::shared_ptr<Matrix> matrix_;
  std::shared_ptr<Level> level_;
  std::unique_ptr<Scoring> scoring_;
  std::unique_ptr<Timer> timer_;
  std::unique_ptr<HighScore> high_score_;
  std::unique_ptr<NextQueue> next_queue_;
  std::shared_ptr<HoldQueue> hold_queue_;
  std::shared_ptr<LinesSent> lines_sent_;
  std::unique_ptr<Goal> goal_;
  std::unique_ptr<TotalLines> total_lines_;
  std::unique_ptr<Moves> moves_;
  std::unique_ptr<Knockout> knockout_;
  std::shared_ptr<MultiPlayer> multi_player_;
  std::vector<PaneInterface*> panes_;
  std::vector<EventListener*> event_listeners_;
  CampaignType type_ = CampaignType::None;
};
