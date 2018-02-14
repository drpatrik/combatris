#pragma once

#include "utility/color.h"
#include "game/coordinates.h"
#include "game/renderer.h"
#include "game/tetromino_rotation_data.h"

#include <memory>

class Tetromino final {
 public:
  enum class Move { None, Left, Right, Down, Rotation };
  enum class Angle { A0, A90, A180, A270 };
  enum class Type { Empty, I, J, L, O, S, T, Z, Filler, Border };

  Tetromino(SDL_Renderer *renderer, Type type, SDL_Color color, const std::vector<TetrominoRotationData>& rotations,
            const std::shared_ptr<SDL_Texture> &texture)
      : renderer_(renderer), type_(type), color_(color), rotations_(rotations), texture_(texture) {}

  Tetromino(const Tetromino &s) = default;

  Tetromino(Tetromino&& other) noexcept { swap(*this, other); }

  Tetromino& operator=(Tetromino other) noexcept {
    swap(*this, other);

    return *this;
  }

  Type type() const { return type_; }

  inline void Render(const Position& pos) const { RenderBlock(renderer_, pos.x(), pos.y(), texture_.get()); }

  inline void RenderGhost(const Position& pos) const { ::RenderGhost(renderer_, pos.x(), pos.y(), color_); }

  const TetrominoRotationData& GetRotationData(Angle angle) const { return rotations_.at(static_cast<size_t>(angle)); }

  void Render(int x, int y, SDL_Texture* texture, Angle angle) const {
    const auto& rotation = rotations_[static_cast<int>(angle)];

    for (size_t row = 0; row < rotation.shape_.size(); ++row) {
      int t_x = x;
      for (size_t col = 0; col < rotation.shape_.at(row).size(); ++col) {
        const auto& shape = rotation.shape_;
        const SDL_Rect rc = { t_x, y, kBlockWidth, kBlockHeight };

        if (shape[row][col] != 0) {
          SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0);
          SDL_RenderFillRect(renderer_, &rc);
          RenderBlock(renderer_, t_x, y, texture);
        }
        t_x += kBlockWidth;
      }
      y += kBlockHeight;
    }
  }

  void Render(int x, int y) const {
    const auto& rotation = rotations_[0];

    x += (((kBlockWidth * 4) - rotation.width_) / 2);
    y -= (((kBlockHeight * 2) - rotation.height_) / 2);

    for (size_t row = 0; row < rotation.shape_.size(); ++row) {
      int t_x = x;
      for (size_t col = 0; col < rotation.shape_.at(row).size(); ++col) {
        const auto& shape = rotation.shape_;

        if (shape[row][col] != 0) {
          RenderBlock(renderer_, t_x, y, texture_.get());
        }
        t_x += kBlockWidth;
      }
      y += kBlockHeight;
    }
  }

  friend void swap(Tetromino& s1, Tetromino& s2) {
    using std::swap;

    swap(s1.renderer_, s2.renderer_);
    swap(s1.type_, s2.type_);
    swap(s1.color_, s2.color_);
    swap(s1.rotations_, s2.rotations_);
    swap(s1.texture_, s2.texture_);
  }

 private:
  SDL_Renderer *renderer_;
  Type type_;
  SDL_Color color_;
  std::vector<TetrominoRotationData> rotations_;
  std::shared_ptr<SDL_Texture> texture_;
};
