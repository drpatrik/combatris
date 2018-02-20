#pragma once

#include "game/panes/pane.h"
#include "game/tetromino_generator.h"

class HoldQueue final : public TextPane, public EventSink {
 public:
   HoldQueue(SDL_Renderer *renderer,
             const std::shared_ptr<TetrominoGenerator> &tetromino_generator,
             const std::shared_ptr<Assets> &assets)
       : TextPane(renderer, kMatrixStartX - kMinoWidth - (kBoxWidth + 8),
                      kMatrixStartY - kMinoHeight, "HOLD", assets),
         tetromino_generator_(tetromino_generator) {}

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
    tetromino_ = old_tetromino_sprite->type();
    wait_for_lock_ = true;

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
    TextPane::Render(delta_time);

    if (Tetromino::Type::Empty == tetromino_) {
      return;
    }
    assets_->GetTetromino(tetromino_)->Render(x_ + 10, y_ + caption_height_ + 15);
  }

  virtual void Reset() override {
    wait_for_lock_ = false;
    tetromino_ = Tetromino::Type::Empty;
  }

 protected:
  bool CanHold() const { return !wait_for_lock_; }

 private:
  bool wait_for_lock_ = false;
  Tetromino::Type tetromino_ = Tetromino::Type::Empty;

  const std::shared_ptr<TetrominoGenerator> tetromino_generator_;
};
