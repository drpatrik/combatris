#include "game/tetrion.h"

#include <iostream>

namespace {

void RenderWindowBackground(SDL_Renderer* renderer) {
  SDL_Rect rc { 0, 0, kWidth, kHeight };

  SDL_SetRenderDrawColor(renderer, 1, 40, 135, 255);
  SDL_RenderFillRect(renderer, &rc);
}

bool RenderAnimations(std::deque<std::shared_ptr<Animation>>& animations, double delta_time, Events& events) {
  for (auto it = animations.begin(); it != animations.end();) {
    (*it)->Render(delta_time);
    if (auto [status, event] = (*it)->IsReady(); status) {
      events.Push(event);
      it = animations.erase(it);
    } else {
      ++it;
    }
  }
  return (animations.size() == 0);
}

template <class T>
void RemoveAnimation(std::deque<std::shared_ptr<Animation>>& animations) {
  animations.erase(std::remove_if(animations.begin(), animations.end(),
    [](const auto &a) { return a->name() == typeid(T).name(); }), animations.end());
}

} // namespace

Tetrion::Tetrion() : events_() {
  window_ = SDL_CreateWindow("COMBATRIS", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, kWidth, kHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  if (nullptr == window_) {
    std::cout << "Failed to create window : " << SDL_GetError() << std::endl;
    exit(-1);
  }
  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
  if (nullptr == renderer_) {
    std::cout << "Failed to create renderer : " << SDL_GetError() << std::endl;
    exit(-1);
  }
  SDL_RaiseWindow(window_);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
  SDL_RenderSetLogicalSize(renderer_, kWidth, kHeight);
  assets_ = std::make_shared<Assets>(renderer_);
  matrix_ = std::make_shared<Matrix>(renderer_, assets_->GetTetrominos());
  AddPane(matrix_.get());
  level_ = std::make_shared<Level>(renderer_, events_, assets_);
  AddPane(level_.get());
  tetromino_generator_ = std::make_shared<TetrominoGenerator>(matrix_, level_, events_, assets_);
  scoring_ = std::make_unique<Scoring>(renderer_, assets_, level_);
  AddPane(scoring_.get());
  high_score_ = std::make_unique<HighScore>(renderer_, assets_);
  AddPane(high_score_.get());
  next_queue_ = std::make_unique<NextQueue>(renderer_, tetromino_generator_, assets_);
  AddPane(next_queue_.get());
  hold_queue_ = std::make_unique<HoldQueue>(renderer_, tetromino_generator_, assets_);
  AddPane(hold_queue_.get());
  total_lines_ = std::make_unique<TotalLines>(renderer_, assets_);
  AddPane(total_lines_.get());
  moves_ = std::make_unique<Moves>(renderer_, assets_);
  AddPane(moves_.get());
  AddAnimation<SplashScreenAnimation>(renderer_, assets_);
  online_game_controller_ = std::make_unique<network::OnlineGameController>(nullptr);
}

Tetrion::~Tetrion() noexcept {
  online_game_controller_->Leave();
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

void Tetrion::GameControl(Controls control_pressed) {
  if (!tetromino_in_play_ || game_paused_) {
    return;
  }
  switch (control_pressed) {
    case Controls::RotateClockwise:
      tetromino_in_play_->RotateClockwise();
      break;
    case Controls::RotateCounterClockwise:
      tetromino_in_play_->RotateCounterClockwise();
      break;
    case Controls::SoftDrop:
      tetromino_in_play_->SoftDrop();
      break;
    case Controls::HardDrop:
      tetromino_in_play_->HardDrop();
      break;
    case Controls::Left:
      tetromino_in_play_->Left();
      break;
    case Controls::Right:
      tetromino_in_play_->Right();
      break;
    case Controls::Hold:
      if (hold_queue_->CanHold()) {
        RemoveAnimation<OnFloorAnimation>(animations_);
        tetromino_in_play_ = hold_queue_->Hold(tetromino_in_play_);
      }
      break;
    default:
      break;
  }
}

void Tetrion::EventHandler(Events& events) {
  online_game_controller_->Dispatch();

  if (events.IsEmpty()) {
    return;
  }
  auto event = events.Pop();

  std::for_each(event_sinks_.begin(), event_sinks_.end(), [&event](const auto& r) { r->Update(event); });

  switch (event.type()) {
    case Event::Type::Pause:
      next_queue_->Hide();
      AddAnimation<PauseAnimation>(renderer_, assets_, unpause_pressed_);
      break;
    case Event::Type::UnPause:
      AddAnimation<CountDownAnimation>(renderer_, assets_, Event::Type::CountdownAfterUnPauseDone);
      break;
    case Event::Type::CountdownAfterUnPauseDone:
      next_queue_->Show();
      level_->ResetTime();
      unpause_pressed_ = game_paused_ = false;
      break;
    case Event::Type::NextTetromino:
      next_queue_->Show();
      tetromino_in_play_ = tetromino_generator_->Get();
      if (tetromino_in_play_->is_game_over()) {
        events.Clear();
        animations_.clear();
        tetromino_in_play_.reset();
        AddAnimation<GameOverAnimation>(renderer_, assets_);
      }
      break;
    case Event::Type::LevelUp:
      AddAnimation<LevelUpAnimation>(renderer_, assets_);
      break;
    case Event::Type::LinesCleared:
      RemoveAnimation<LinesClearedAnimation>(animations_);
      AddAnimation<LinesClearedAnimation>(renderer_, assets_, event.lines_cleared_);
      break;
    case Event::Type::CalculatedScore:
      AddAnimation<ScoreAnimation>(renderer_, assets_, event.pos_, event.score_);
      break;
    case Event::Type::NewGame:
      events.Clear();
      next_queue_->Hide();
      tetromino_generator_->Reset();
      unpause_pressed_ = game_paused_ = false;
      std::for_each(panes_.begin(), panes_.end(), [](const auto& r) { r->Reset(); });
      AddAnimation<CountDownAnimation>(renderer_, assets_, Event::Type::NextTetromino);
      break;
    case Event::Type::PerfectClear:
      std::cout << "Perfect Clear" << std::endl;
      break;
    case Event::Type::OnFloor:
      AddAnimation<OnFloorAnimation>(renderer_, assets_, tetromino_in_play_);
      break;
    case Event::Type::Falling:
      RemoveAnimation<OnFloorAnimation>(animations_);
      break;
    case Event::Type::SendLines:
      break;
    case Event::Type::GotLines:
      break;
    default:
      break;
  }
}

void Tetrion::Render(double delta_time) {
  SDL_RenderClear(renderer_);

  RenderWindowBackground(renderer_);

  std::for_each(panes_.begin(), panes_.end(), [delta_time](const auto& pane) { pane->Render(delta_time); });

  RenderAnimations(animations_, delta_time, events_);

  SDL_RenderPresent(renderer_);
}

void Tetrion::Update(double delta_time) {
  EventHandler(events_);
  if (!game_paused_) {
    if (tetromino_in_play_ && tetromino_in_play_->Down(delta_time) == TetrominoSprite::State::Commited) {
      tetromino_in_play_.reset();
      events_.Push(Event::Type::NextTetromino);
    }
  }
  Render(delta_time);
}
