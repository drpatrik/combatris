#pragma once

#include "game/panes/pane.h"
#include "game/tetromino_sprite.h"

class HoldQueue final : public TextPane, public EventListener {
 public:
  static const int kX = kMatrixStartX - kMinoWidth - (kBoxWidth + kSpace);
  static const int kY = kMatrixStartY - kMinoHeight;

  HoldQueue(SDL_Renderer *renderer, const std::shared_ptr<Assets> &assets)
      : TextPane(renderer, kX, kY, "HOLD", assets) {
    checkmark_texture_ = utility::Texture(assets_->GetTexture(Assets::Type::Checkmark));
    checkmark_texture_.SetXY(kX + 90, kY - 10);
  }

  Tetromino::Type Hold(const std::shared_ptr<TetrominoSprite>& old_tetromino_sprite) {
    Tetromino::Type type = tetromino_;

    tetromino_ = old_tetromino_sprite->tetromino().type();
    ticks_ = 0.0;
    wait_for_lock_ = true;
    display_checkmark_ = true;

    return type;
  }

  virtual void Update(const Event& event) override {
    if (!event.Is(Event::Type::CanHold)) {
      return;
    }
    wait_for_lock_ = false;
  }

  virtual void Render(double delta_time) override {
    const auto kDisplayTime = 0.4;

    TextPane::Render(delta_time);

    if (Tetromino::Type::Empty != tetromino_) {
      assets_->GetTetromino(tetromino_)->RenderTetromino(x_ + 10, y_ + caption_texture_.height() + 15);
    }
    ticks_ += delta_time;
    if (ticks_ >= kDisplayTime) {
      display_checkmark_ = false;
    }
    if (display_checkmark_) {
      Pane::RenderCopy(checkmark_texture_, checkmark_texture_);
    }
  }

  virtual void Reset() override {
    wait_for_lock_ = false;
    tetromino_ = Tetromino::Type::Empty;
  }

  inline bool CanHold() const { return !wait_for_lock_; }

 private:
  double ticks_ = 0.0;
  bool wait_for_lock_ = false;
  bool display_checkmark_ = false;
  Tetromino::Type tetromino_ = Tetromino::Type::Empty;
  utility::Texture checkmark_texture_;
};
