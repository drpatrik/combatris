#pragma once

#include "game/panes/pane.h"
#include "game/tetromino_generator.h"

class HoldQueue final : public TextPane, public EventListener {
 public:
  static const int kX = kMatrixStartX - kMinoWidth - (kBoxWidth + kSpace);
  static const int kY = kMatrixStartY - kMinoHeight;

  HoldQueue(SDL_Renderer *renderer,
            const std::shared_ptr<TetrominoGenerator> &tetromino_generator,
            const std::shared_ptr<Assets> &assets)
      : TextPane(renderer, kX, kY, "HOLD", assets), tetromino_generator_(tetromino_generator) {
    rc_.x = kX + 90;
    rc_.y = kY - 10;
    std::tie(checkmark_texture_, rc_.w, rc_.h) = assets_->GetTexture(Assets::Type::Checkmark);
  }

  std::shared_ptr<TetrominoSprite> Hold(const std::shared_ptr<TetrominoSprite>& old_tetromino_sprite) {
    if (!CanHold()) {
      return old_tetromino_sprite;
    }
    std::shared_ptr<TetrominoSprite> tetromino_sprite;

    if (Tetromino::Type::Empty == tetromino_) {
      tetromino_sprite = tetromino_generator_->Get();
    } else {
      tetromino_sprite = tetromino_generator_->Get(tetromino_);
    }
    tetromino_ = old_tetromino_sprite->tetromino().type();
    ticks_ = 0.0;
    wait_for_lock_ = true;
    display_checkmark_ = true;

    return tetromino_sprite;
  }

  virtual void Update(const Event& event) override {
    if (!event.Is(Event::Type::NextTetromino)) {
      return;
    }
    wait_for_lock_ = false;
  }

  std::shared_ptr<TetrominoSprite> Get() { return tetromino_generator_->Get(tetromino_); }

  virtual void Render(double delta_time) override {
    const auto kDisplayTime = 0.4;

    TextPane::Render(delta_time);

    if (Tetromino::Type::Empty != tetromino_) {
      assets_->GetTetromino(tetromino_)->RenderTetromino(x_ + 10, y_ + caption_height_ + 15);
    }
    ticks_ += delta_time;
    if (ticks_ >= kDisplayTime) {
      display_checkmark_ = false;
    }
    if (display_checkmark_) {
      Pane::RenderCopy(checkmark_texture_.get(), rc_);
    }
  }

  virtual void Reset() override {
    wait_for_lock_ = false;
    tetromino_ = Tetromino::Type::Empty;
  }

  bool CanHold() const { return !wait_for_lock_; }

 private:
  double ticks_ = 0.0;
  bool wait_for_lock_ = false;
  bool display_checkmark_ = false;
  Tetromino::Type tetromino_ = Tetromino::Type::Empty;
  SDL_Rect rc_;
  std::shared_ptr<SDL_Texture> checkmark_texture_;
  const std::shared_ptr<TetrominoGenerator> tetromino_generator_;
};
