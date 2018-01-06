#include "game/board.h"

#include <iostream>

Board::Board() {
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
  tetromino_in_play_ = std::make_unique<TetrominoSprite>(*asset_manager_->GetTetromino(current_tetromino_));
}

Board::~Board() noexcept {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

void Board::Up() {
  if (!tetromino_in_play_) {
    return;
  }
  tetromino_in_play_->Rotate();
}

void Board::Down() {
  if (!tetromino_in_play_) {
    return;
  }
}

void Board::Left() {
  if (!tetromino_in_play_) {
    return;
  }
  int n = static_cast<int>(current_tetromino_) - 1;

  if (n < 1) {
    n = kNumTetrominos - 1;
  }
  current_tetromino_ = static_cast<Tetromino::Type>(n);
  tetromino_in_play_ = std::make_unique<TetrominoSprite>(*asset_manager_->GetTetromino(current_tetromino_));
}

void Board::Right() {
  if (!tetromino_in_play_) {
    return;
  }
  auto n = static_cast<size_t>(current_tetromino_) + 1;

  if (n > kNumTetrominos - 1) {
    n = 1;
  }
  current_tetromino_ = static_cast<Tetromino::Type>(n);
  tetromino_in_play_ = std::make_unique<TetrominoSprite>(*asset_manager_->GetTetromino(current_tetromino_));
}

void Board::Render(double) {
  SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0);
  SDL_RenderClear(renderer_);

  if (tetromino_in_play_) {
    tetromino_in_play_->RenderOutline(8, 2);
    tetromino_in_play_->Render(2, 2);
  }

  SDL_RenderPresent(renderer_);
}
