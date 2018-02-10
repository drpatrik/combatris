#include "game/board.h"

#include <iostream>

namespace {

void RenderWindowBackground(SDL_Renderer* renderer) {
  SDL_Rect rc { 0, 0, kWidth, kHeight };

  SDL_SetRenderDrawColor(renderer, 1, 40, 135, 255);
  SDL_RenderFillRect(renderer, &rc);
}

bool RenderAnimations(std::deque<std::shared_ptr<Animation>>& animations, double delta_time) {
  for (auto it = std::begin(animations); it != std::end(animations);) {
    (*it)->Render(delta_time);
    if ((*it)->IsReady()) {
      it = animations.erase(it);
    } else {
      ++it;
    }
  }

  return (animations.size() == 0);
}

} // namespace

Board::Board() : events_() {
  window_ = SDL_CreateWindow("Combatris", SDL_WINDOWPOS_UNDEFINED,
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
  panes_.push_back(matrix_.get());
  level_ = std::make_shared<Level>(renderer_, events_, assets_);
  panes_.push_back(level_.get());
  event_sinks_.push_back(level_.get());
  scoring_ = std::make_unique<Scoring>(renderer_, assets_, level_);
  event_sinks_.push_back(scoring_.get());
  panes_.push_back(scoring_.get());
  tetromino_generator_ = std::make_shared<TetrominoGenerator>(matrix_, level_, events_, assets_);
  next_piece_ = std::make_unique<NextPiece>(renderer_, tetromino_generator_, assets_);
  panes_.push_back(next_piece_.get());
  hold_piece_ = std::make_unique<HoldPiece>(renderer_, tetromino_generator_, assets_);
  panes_.push_back(hold_piece_.get());
  event_sinks_.push_back(hold_piece_.get());
  total_lines_ = std::make_unique<TotalLines>(renderer_, assets_);
  panes_.push_back(total_lines_.get());
  event_sinks_.push_back(total_lines_.get());
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

void Board::Render(double delta_time) {
  SDL_RenderClear(renderer_);

  RenderWindowBackground(renderer_);

  std::for_each(panes_.begin(), panes_.end(), [](const auto& r) { r->Render(); });

  RenderAnimations(animations_, delta_time);

  SDL_RenderPresent(renderer_);
}

void Board::Update(double delta_time) {
  if (!events_.IsEmpty()) {
    auto event = events_.Pop();

    std::for_each(event_sinks_.begin(), event_sinks_.end(), [&event](const auto& r) { r->Update(event); });

    switch (event.type()) {
      case Event::Type::Pause:
        game_paused_ = !game_paused_;
        if (game_paused_) {
          std::cout << "Game Paused" << std::endl;
        } else {
          level_->ResetTime();
          std::cout << "Running" << std::endl;
        }
        break;
      case Event::Type::CountDown:
        break;
      case Event::Type::NextPiece:
        tetromino_in_play_ = tetromino_generator_->Get();
        if (tetromino_in_play_->is_game_over()) {
          tetromino_in_play_.reset();
          events_.PushFront(Event::Type::GameOver);
        }
        break;
      case Event::Type::LevelUp:
        std::cout << "Level Up" << std::endl;
        break;
      case Event::Type::GameOver:
        events_.Clear();
        animations_.clear();
        tetromino_in_play_.reset();
        std::cout << "Game Over" << std::endl;
        break;
      case Event::Type::NewGame:
        events_.Clear();
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
        std::cout << "Floor reached" << std::endl;
        break;
      case Event::Type::InTransit:
        std::cout << "In transit" << std::endl;
        break;
      case Event::Type::SendGarbage:
        break;
      case Event::Type::GotGarbage:
        break;
      default:
        break;
    }
  }
  if (!game_paused_) {
    if (tetromino_in_play_ && tetromino_in_play_->Down(delta_time) == TetrominoSprite::Status::Commited) {
      tetromino_in_play_.reset();
      events_.Push(Event::Type::NextPiece);
    }
  }
  Render(delta_time);
}
