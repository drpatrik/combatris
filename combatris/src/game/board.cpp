#include "game/board.h"

#include <iostream>

namespace {

void RenderWindowBackground(SDL_Renderer* renderer) {
  SDL_Rect rc { 0, 0, kWidth, kHeight };

  SDL_SetRenderDrawColor(renderer, 1, 40, 135, 255);
  SDL_RenderFillRect(renderer, &rc);
}

void RenderBorder(SDL_Renderer* renderer, SDL_Texture* frame_texture) {
  const int kRightFrameRow = (kFrameRows - 1) * kBlockHeight;
  const int kLowerFrameCol = (kFrameCols - 1) * kBlockWidth;

  int x = kFrameStartX;
  int y = kFrameStartY;

  for (int col = 0; col < kFrameCols; ++col) {
    RenderBlock(renderer,x, y, frame_texture);
    RenderBlock(renderer,x, y + kRightFrameRow, frame_texture);
    x += kBlockWidth;
  }

  x = kFrameStartX;
  y = kFrameStartY;

  for (int row = 0; row < kFrameRows; ++row) {
    RenderBlock(renderer,x, y, frame_texture);
    RenderBlock(renderer,x + kLowerFrameCol, y, frame_texture);
    y += kBlockHeight;
  }
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
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
  SDL_RenderSetLogicalSize(renderer_, kWidth, kHeight);
  asset_manager_ = std::make_shared<AssetManager>(renderer_);
  matrix_ = std::make_shared<Matrix>(renderer_, events_, asset_manager_->GetTetrominos());
  level_ = std::make_shared<Level>(renderer_, events_, asset_manager_, 10, 10);
  scoring_ = std::make_shared<Scoring>(*level_);
  tetromino_generator_ = std::make_unique<TetrominoGenerator>(matrix_, *level_, asset_manager_);
  NewGame();
}

Board::~Board() noexcept {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

void Board::NewGame() {
  events_.Push(EventType::NewGame);
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
      break;
    case Controls::Pause:
      break;
  }
}

void Board::Render(double /*delta_time*/) {
  SDL_RenderClear(renderer_);

  RenderWindowBackground(renderer_);
  RenderText(renderer_, 750, 10 , asset_manager_->GetFont(Font::Normal), "Next: ", Color::White);

  tetromino_generator_->next()->Render(750, 50);
  scoring_->Render();
  level_->Render();
  RenderBorder(renderer_, asset_manager_->GetBorderTexture());
  matrix_->Render();

  SDL_RenderPresent(renderer_);
}

void Board::Update(double delta_time) {
  auto event = events_.Pop();

  switch (event.type()) {
    case EventType::None:
      break;
    case EventType::LinesCleared:
      level_->Update(event);
      scoring_->Update(event);
      // Passby on purpose
    case EventType::NextPiece:
      tetromino_in_play_ = tetromino_generator_->Get();
      break;
    case EventType::LevelUp:
      std::cout << "Level Up" << std::endl;
      break;
    case EventType::GameOver:
      tetromino_in_play_.reset();
      std::cout << "Game Over" << std::endl;
      break;
    case EventType::NewGame:
      events_.NewGame();
      matrix_->NewGame();
      scoring_->NewGame();
      tetromino_generator_->NewGame();
      tetromino_in_play_ = tetromino_generator_->Get();
      break;
  };
  if (tetromino_in_play_) {
    tetromino_in_play_->Down(delta_time);
  }
  Render(delta_time);
}
