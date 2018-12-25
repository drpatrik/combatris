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
  enum class Type { Empty, I, J, L, O, S, T, Z, Solid, Bomb, Border };

  Tetromino(SDL_Renderer *renderer, Type type, SDL_Color color, const std::vector<TetrominoRotationData>& rotations,
            const std::shared_ptr<SDL_Texture> &texture)
      : renderer_(renderer), type_(type), color_(color), rotations_(rotations), texture_(texture) {}

  Tetromino(const Tetromino&) = delete;

  inline Type type() const { return type_; }

  inline SDL_Texture* texture() const { return texture_.get(); }

  inline void Render(int x, int y) const { RenderMino(renderer_, x, y, texture_.get()); }

  inline void Render(const Position& pos) const { RenderMino(renderer_, pos.x(), pos.y(), texture_.get()); }

  inline void Render(const Position& pos, int w, int h) const { RenderMino(renderer_, pos.x(), pos.y(), w, h, texture_.get()); }

  inline void RenderGhost(const Position& pos) const { ::RenderGhost(renderer_, pos.x(), pos.y(), color_); }

  const TetrominoRotationData& GetRotationData(Angle angle) const { return rotations_.at(static_cast<size_t>(angle)); }

  void Render(int x, int y, SDL_Texture* texture, Angle angle) const {
    const auto& rotation = rotations_[static_cast<int>(angle)];

    for (int row = 0; row < static_cast<int>(rotation.shape_.size()); ++row) {
      auto t_x = x;
      for (int col = 0; col < static_cast<int>(rotation.shape_[row].size()); ++col) {
        const auto& shape = rotation.shape_;
        const SDL_Rect rc = { t_x, y, kMinoWidth, kMinoHeight };

        if (shape[row][col] != 0) {
          SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0);
          SDL_RenderFillRect(renderer_, &rc);
          RenderMino(renderer_, t_x, y, texture);
        }
        t_x += kMinoWidth;
      }
      y += kMinoHeight;
    }
  }

  void RenderTetromino(int x, int y) const {
    const auto& rotation = rotations_[0];

    x += (((kMinoWidth * 4) - rotation.width_) / 2);
    y -= (((kMinoHeight * 2) - rotation.height_) / 2);

    for (int row = 0; row < static_cast<int>(rotation.shape_.size()); ++row) {
      auto t_x = x;
      for (int col = 0; col < static_cast<int>(rotation.shape_[row].size()); ++col) {
        const auto& shape = rotation.shape_;

        if (shape[row][col] != 0) {
          RenderMino(renderer_, t_x, y, texture_.get());
        }
        t_x += kMinoWidth;
      }
      y += kMinoHeight;
    }
  }

 private:
  SDL_Renderer *renderer_;
  Type type_;
  SDL_Color color_;
  std::vector<TetrominoRotationData> rotations_;
  std::shared_ptr<SDL_Texture> texture_;
};

const int kEmptyID = static_cast<int>(Tetromino::Type::Empty);
const int kBombID = static_cast<int>(Tetromino::Type::Bomb);
const int kBorderID = static_cast<int>(Tetromino::Type::Border);
const int kSolidID = static_cast<int>(Tetromino::Type::Solid);
const int kGhostAddOn = kBorderID + 1;
