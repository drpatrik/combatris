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

}

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
  tetromino_generator_ = std::make_unique<TetrominoGenerator>(renderer_, asset_manager_);
  tetromino_in_play_ = tetromino_generator_->Get(current_tetromino_);
  matrix_ = std::make_shared<Matrix>(renderer_, *tetromino_generator_);
  border_texture_ = asset_manager_->GetSprite(kBorderSpriteID).get();
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
  SDL_RenderClear(renderer_);

  RenderWindowBackground(renderer_);
  RenderBorder(renderer_, border_texture_);
  matrix_->Render();

  SDL_RenderPresent(renderer_);
}
