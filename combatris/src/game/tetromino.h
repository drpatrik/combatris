#pragma once

#include "tools/color.h"
#include "game/coordinates.h"
#include "game/tetromino_rotation_data.h"

#include <memory>

class Tetromino final {
 public:
  enum class Angle { A0, A90, A180, A270 };
  enum class Type { Invalid, I, J, L, O, S, T, Z };

  Tetromino(SDL_Renderer *renderer, Type type, SDL_Color color, const std::vector<TetrominoRotationData>& rotations,
            const std::shared_ptr<SDL_Texture> &tetromino)
      : renderer_(renderer), type_(type), color_(color), rotations_(rotations), tetromino_(tetromino) {}

  Tetromino(const Tetromino &s) : renderer_(s.renderer_), type_(s.type_), color_(s.color_), rotations_(s.rotations_), tetromino_(s.tetromino_) {}

  Tetromino(Tetromino&& other) noexcept { swap(*this, other); }

  Tetromino& operator=(Tetromino other) noexcept {
    swap(*this, other);

    return *this;
  }

  inline void RenderXY(int x, int y) const {
    SDL_Rect dest_rc {x, y, kBlockWidth, kBlockHeight };

    SDL_RenderCopy(renderer_, tetromino_.get(), nullptr, &dest_rc);
  }

  void RenderGhostXY(int x, int y) const {
    SDL_Rect rc { x, y, kBlockWidth, kBlockHeight };

    SDL_SetRenderDrawColor(renderer_, color_.r, color_.g, color_.b, color_.a);
    SDL_RenderFillRect(renderer_, &rc);

    rc = { x + 1, y + 1, kBlockWidth - 2, kBlockHeight - 2 };

    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0);
    SDL_RenderFillRect(renderer_, &rc);
  }

  inline void Render(int row, int col) const { RenderXY(row_to_pixel(row), col_to_pixel(col)); }

  inline void RenderGhost(int row, int col) const { RenderGhostXY(row_to_pixel(row), col_to_pixel(col)); }

  const TetrominoRotationData& GetRotationData(Angle angle) const { return rotations_.at(static_cast<size_t>(angle)); }

  friend void swap(Tetromino& s1, Tetromino& s2) {
    using std::swap;

    swap(s1.renderer_, s2.renderer_);
    swap(s1.type_, s2.type_);
    swap(s1.color_, s2.color_);
    swap(s1.rotations_, s2.rotations_);
    swap(s1.tetromino_, s2.tetromino_);
  }

 private:
  SDL_Renderer *renderer_;
  Type type_;
  SDL_Color color_;
  std::vector<TetrominoRotationData> rotations_;
  std::shared_ptr<SDL_Texture> tetromino_;
};
