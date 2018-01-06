#pragma once

#include "game/constants.h"
#include "game/tetromino_metadata.h"

#include <memory>

class Tetromino final {
 public:
  enum class Angle { A0, A90, A180, A270 };
  enum class Type { Invalid, I, J, L, O, S, T, Z };

  Tetromino(SDL_Renderer *renderer, Type type, const std::vector<TetrominoMetadata>& metadata,
            const std::shared_ptr<SDL_Texture> &tetromino)
      : renderer_(renderer), type_(type), metadata_(metadata), tetromino_(tetromino) {}

  Tetromino(const Tetromino &s) : renderer_(s.renderer_), type_(s.type_), metadata_(s.metadata_), tetromino_(s.tetromino_) {}

  Tetromino(Tetromino&& other) noexcept { swap(*this, other); }

  Tetromino& operator=(Tetromino other) noexcept {
    swap(*this, other);

    return *this;
  }

  void Render(int x, int y) const;

  void Render(int x, int y, Angle angle) const;

  void RenderOutline(int x, int y, Angle angle) const;

  Type type() const { return type_; }

  int block_width() const { return kBlockWidth; }

  int block_height() const { return kBlockHeight; }

  friend void swap(Tetromino& s1, Tetromino& s2) {
    using std::swap;

    swap(s1.renderer_, s2.renderer_);
    swap(s1.type_, s2.type_);
    swap(s1.metadata_, s2.metadata_);
    swap(s1.tetromino_, s2.tetromino_);
  }

 private:
  SDL_Renderer *renderer_;
  Type type_;
  std::vector<TetrominoMetadata> metadata_;
  std::shared_ptr<SDL_Texture> tetromino_;
};
