#pragma once

#include "game/panes/pane.h"
#include "game/tetromino_sprite.h"

class HoldQueue final : public TextPane, public EventListener {
 public:
  static constexpr auto kCheckmarkDisplayTime = 0.4;
  static const int kX = kMatrixStartX - kMinoWidth - (kBoxWidth + kSpace);
  static const int kY = kMatrixStartY - kMinoHeight;

  HoldQueue(SDL_Renderer *renderer, const std::shared_ptr<Assets> &assets)
      : TextPane(renderer, kX, kY, "HOLD", assets), checkmark_texture_(utility::Texture(assets_->GetTexture(Assets::Type::Checkmark))) {
    checkmark_texture_.SetXY(kX + 90, kY - 10);
  }

  Tetromino::Type Hold(const std::shared_ptr<TetrominoSprite>& old_tetromino_sprite) {
    auto type = tetromino_;

    tetromino_ = old_tetromino_sprite->tetromino().type();
    ticks_ = 0.0;
    can_hold_ = false;

    return type;
  }

  virtual void Update(const Event& event) override {
    if (!event.Is(Event::Type::CanHold)) {
      return;
    }
    can_hold_ = true;
  }

  virtual void Render(double delta_time) override {
    TextPane::Render(delta_time);

    if (Tetromino::Type::Empty == tetromino_) {
      return;
    }
    assets_->GetTetromino(tetromino_)->RenderTetromino(x_ + 10, y_ + caption_texture_.height() + 15);
    ticks_ += delta_time;
    if (ticks_ <= kCheckmarkDisplayTime) {
      Pane::RenderCopy(checkmark_texture_, checkmark_texture_);
    }
  }

  virtual void Reset() override {
    ticks_ = 1.0;
    can_hold_ = true;
    tetromino_ = Tetromino::Type::Empty;
  }

  inline bool CanHold() const { return can_hold_; }

 private:
  double ticks_ = 1.0;
  bool can_hold_ = false;
  utility::Texture checkmark_texture_;
  Tetromino::Type tetromino_ = Tetromino::Type::Empty;
};
