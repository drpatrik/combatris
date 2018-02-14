#include "game/board.h"

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
                                  [](const auto &a) {
                                    return a->name() == typeid(T).name();
                                  }),
                   animations.end());
}

} // namespace

Board::Board() : events_() {
  window_ = SDL_CreateWindow("COMBATRIS", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, kWidth, kHeight, SDL_WINDOW_RESIZABLE);
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
  next_piece_ = std::make_unique<NextPiece>(renderer_, tetromino_generator_, assets_);
  AddPane(next_piece_.get());
  hold_piece_ = std::make_unique<HoldPiece>(renderer_, tetromino_generator_, assets_);
  AddPane(hold_piece_.get());
  total_lines_ = std::make_unique<TotalLines>(renderer_, assets_);
  AddPane(total_lines_.get());
  moves_ = std::make_unique<Moves>(renderer_, assets_);
  AddPane(moves_.get());
  AddAnimation<SplashScreenAnimation>(renderer_, assets_);
}

Board::~Board() noexcept {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

void Board::GameControl(Controls control_pressed) {
  if (!tetromino_in_play_) {
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
    case Controls::HoldPiece:
      if (hold_piece_->CanHold()) {
        tetromino_in_play_ = hold_piece_->Hold(tetromino_in_play_->type());
      }
      break;
  }
}

void Board::EventHandler(Events& events) {
  if (events.IsEmpty()) {
    return;
  }
  auto event = events.Pop();

  std::for_each(event_sinks_.begin(), event_sinks_.end(), [&event](const auto& r) { r->Update(event); });

  switch (event.type()) {
    case Event::Type::Pause:
      next_piece_->Hide();
      AddAnimation<PauseAnimation>(renderer_, assets_, game_paused_);
      break;
    case Event::Type::UnPause:
      AddAnimation<CountDownAnimation>(renderer_, assets_, Event::Type::CountdownAnimationDone);
      break;
    case Event::Type::CountdownAnimationDone:
      level_->ResetTime();
      next_piece_->Show();
      break;
    case Event::Type::NextPiece:
      tetromino_in_play_ = tetromino_generator_->Get();
      if (tetromino_in_play_->is_game_over()) {
        tetromino_in_play_.reset();
        events.PushFront(Event::Type::GameOver);
      }
      break;
    case Event::Type::LevelUp:
      AddAnimation<LevelUpAnimation>(renderer_, assets_);
      break;
    case Event::Type::Score:
      AddAnimation<ScoreAnimation>(renderer_, assets_, event.pos_, event.score_);
      break;
    case Event::Type::GameOver:
      events.Clear();
      animations_.clear();
      tetromino_in_play_.reset();
      AddAnimation<GameOverAnimation>(renderer_, assets_);
      break;
    case Event::Type::NewGame:
      AddAnimation<CountDownAnimation>(renderer_, assets_, Event::Type::ResetGame);
      break;
    case Event::Type::ResetGame:
      events.Clear();
      next_piece_->Show();
      tetromino_generator_->Reset();
      game_paused_ = false;
      std::for_each(panes_.begin(), panes_.end(), [](const auto& r) { r->Reset(); });
      tetromino_in_play_ = tetromino_generator_->Get();
      break;
    case Event::Type::PerfectClear:
      std::cout << "Perfect Clear" << std::endl;
      break;
    case Event::Type::FloorReached:
      AddAnimation<FloorReachedAnimation>(renderer_, assets_, tetromino_in_play_);
      break;
    case Event::Type::FloorLeft:
      RemoveAnimation<FloorReachedAnimation>(animations_);
      break;
    case Event::Type::SendLines:
      break;
    case Event::Type::GotLines:
      break;
    default:
      break;
  }
}

void Board::Render(double delta_time) {
  SDL_RenderClear(renderer_);

  RenderWindowBackground(renderer_);

  std::for_each(panes_.begin(), panes_.end(), [delta_time](const auto& r) { r->Render(delta_time); });

  RenderAnimations(animations_, delta_time, events_);

  SDL_RenderPresent(renderer_);
}

void Board::Update(double delta_time) {
  EventHandler(events_);
  if (!game_paused_) {
    if (tetromino_in_play_ && tetromino_in_play_->Down(delta_time) == TetrominoSprite::Status::Commited) {
      tetromino_in_play_.reset();
      events_.Push(Event::Type::NextPiece);
    }
  }
  Render(delta_time);
}
