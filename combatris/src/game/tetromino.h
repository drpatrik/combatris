#pragma once

#include "game/constants.h"

#include <memory>

#include <SDL.h>

class Tetromino final {
 public:
  enum class DisplayAngle { D0, D90, D180, D270 };
  enum class Type { I_Block, J_Block, L_Block, O_Block, S_Block, T_Block, Z_Block };

  Tetromino(SDL_Renderer *renderer, Type type,
            const std::shared_ptr<SDL_Texture> &tetromino)
      : renderer_(renderer), type_(type), tetromino_(tetromino) {}

  Tetromino(const Tetromino &s) : renderer_(s.renderer_), type_(s.type_), tetromino_(s.tetromino_) {}

  Tetromino(Tetromino&& other) noexcept { swap(*this, other); }

  Tetromino& operator=(Tetromino other) noexcept {
    swap(*this, other);

    return *this;
  }

  void Render(int x, int y) const;

  void Render(int x, int y, DisplayAngle angle) const;

  Type type() const { return type_; }

  int block_width() const { return width_; }

  int block_height() const { return height_; }

  friend void swap(Tetromino& s1, Tetromino& s2) {
    using std::swap;

    swap(s1.renderer_, s2.renderer_);
    swap(s1.type_, s2.type_);
    swap(s1.tetromino_, s2.tetromino_);
  }

 private:
  SDL_Renderer *renderer_;
  Type type_;
  std::shared_ptr<SDL_Texture> tetromino_;
  const int width_ = BlockWidth;
  const int height_ =BlockHeight;
};
