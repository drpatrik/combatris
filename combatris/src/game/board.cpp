#include "game/board.h"

#include <iostream>

namespace {

const std::vector<Tetromino::Angle> kAngles = {Tetromino::Angle::A0, Tetromino::Angle::A90, Tetromino::Angle::A180, Tetromino::Angle::A270 };

}

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
}

Board::~Board() noexcept {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

void Board::Up() {
  int i = static_cast<int>(current_angle_);

  i++;
  current_angle_ = static_cast<Tetromino::Angle>(i % kAngles.size());
}

void Board::Down() {}

void Board::Left() {
  size_t i = static_cast<size_t>(current_tetromino_);

  i++;
  if (i > kNumTetrominos) {
    i = 1;
  }
  current_tetromino_ = static_cast<Tetromino::Type>(i);
}

void Board::Right() {
  int i = static_cast<int>(current_tetromino_);

  i--;
  if (i < 1) {
    i = kNumTetrominos;
  }
  current_tetromino_ = static_cast<Tetromino::Type>(i);
}

void Board::Render(double) {
  SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0);
  SDL_RenderClear(renderer_);

  auto sprite = asset_manager_->GetTetromino(current_tetromino_);

  int x = kBlockWidth;
  int y = kBlockHeight;

  sprite->Render(x, y, current_angle_);

  SDL_RenderPresent(renderer_);
}
