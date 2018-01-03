#pragma once

#include <memory>

#include <SDL.h>

enum class SpriteID { I_Block, J_Block, L_Block, O_Block, S_Block, T_Block, Z_Block };

class Sprite final {
 public:
  Sprite(SpriteID id, const std::shared_ptr<SDL_Texture> &sprite) : id_(id), sprite_(sprite) {
    Uint32 format;
    int access;

    SDL_QueryTexture(sprite_.get(), &format, &access, &width_, &height_);
  }

  Sprite(const Sprite &s) : id_(s.id_), sprite_(s.sprite_), width_(s.width_), height_(s.height_) {}

  Sprite(Sprite&& other) { swap(*this, other); }

  Sprite& operator=(Sprite other) {
    swap(*this, other);

    return *this;
  }

  SpriteID id() const { return id_; }

  SDL_Texture* operator()() const { return sprite_.get(); }

  SDL_Texture* sprite() const { return sprite_.get(); }

  int width() const { return width_; }

  int height() const { return height_; }

  friend void swap(Sprite& s1, Sprite& s2) {
    using std::swap;

    swap(s1.id_, s2.id_);
    swap(s1.sprite_, s2.sprite_);
    swap(s1.width_, s2.width_);
    swap(s1.height_, s2.height_);
  }

 private:
  SpriteID id_;
  std::shared_ptr<SDL_Texture> sprite_;
  int width_ = 0;
  int height_ = 0;
};
