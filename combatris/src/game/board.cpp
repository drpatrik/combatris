#include "game/board.h"

#include <iostream>

Board::Board() {
  window_ = SDL_CreateWindow("Combatris", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, kWidth, kHeight, SDL_WINDOW_RESIZABLE);
  if (nullptr == window_) {
    std::cout << "Failed to create window : " << SDL_GetError() << std::endl;
    exit(-1);
  }
  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (nullptr == renderer_) {
    std::cout << "Failed to create renderer : " << SDL_GetError() << std::endl;
    exit(-1);
  }
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
  SDL_RenderSetLogicalSize(renderer_, kWidth, kHeight);

  asset_manager_ = std::make_shared<AssetManager>(renderer_);
  tetromino_generator_ = std::make_shared<TetrominoGenerator>(renderer_, *asset_manager_);
  tetromino_in_play_ = tetromino_generator_->Get(current_tetromino_);
}

Board::~Board() noexcept {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

void Board::Up() {
  if (!tetromino_in_play_) {
    return;
  }
  tetromino_in_play_->RotateRight();
}

void Board::Down() {
  if (!tetromino_in_play_) {
    return;
  }
  auto n = static_cast<size_t>(current_tetromino_) + 1;

  if (n > kNumTetrominos - 1) {
    n = 1;
  }
  current_tetromino_ = static_cast<Tetromino::Type>(n);
  tetromino_in_play_ = tetromino_generator_->Get(current_tetromino_);
}

void Board::Left() {
  if (!tetromino_in_play_) {
    return;
  }
  tetromino_in_play_->Left();
}

void Board::Right() {
  if (!tetromino_in_play_) {
    return;
  }
  tetromino_in_play_->Right();
}

void Board::Render(double) {
  SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0);
  SDL_RenderClear(renderer_);

  // Draw Grid
  auto gray = GetColor(Color::Gray);

  SDL_SetRenderDrawColor(renderer_, gray.r, gray.g, gray.b, gray.a);
  SDL_Rect rc { 0, 0, kWidth, kHeight };
  SDL_RenderFillRect(renderer_, &rc);

  SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0);
  int y = 0;

  for (int row = 0; row < kRows; ++row) {
    int x = 0;
    for (int col = 0; col < kCols; ++col) {
      SDL_Rect rc = { x + 1, y + 1, kBlockWidth - 2, kBlockHeight - 2 };
      SDL_RenderFillRect(renderer_, &rc);
      x += kBlockWidth;
    }
    y += kBlockHeight;
  }
  if (tetromino_in_play_) {
    tetromino_in_play_->Render();
  }

  SDL_RenderPresent(renderer_);
}
