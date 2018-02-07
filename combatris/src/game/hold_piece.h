#pragma once

#include "game/pane.h"
#include "game/tetromino_generator.h"

class HoldPiece final : public TextPane {
 public:
   HoldPiece(SDL_Renderer *renderer,
             const std::shared_ptr<TetrominoGenerator> &tetromino_generator,
             const std::shared_ptr<Assets> &assets)
       : TextPane(renderer, kMatrixStartX - kBlockWidth - 165,
                      kMatrixStartY - kBlockHeight, "HOLD", assets),
         tetromino_generator_(tetromino_generator) {}

   std::unique_ptr<TetrominoSprite> Hold(Tetromino::Type tetromino) {
     std::unique_ptr<TetrominoSprite> tetromino_sprite;

     if (Tetromino::Type::Empty == tetromino_) {
       tetromino_sprite = tetromino_generator_->Get();
     } else {
       tetromino_sprite = tetromino_generator_->Get(tetromino_);
     }
     tetromino_ = tetromino;
     wait_for_lock_ = true;

     return tetromino_sprite;
  }

  void NewGame() {
    wait_for_lock_ = false;
    tetromino_ = Tetromino::Type::Empty;
  }

  bool CanHold() const { return !wait_for_lock_; }

  void Update(const Event& event) { wait_for_lock_ = !(event.type() == Event::Type::NextPiece); }

  std::unique_ptr<TetrominoSprite> Get() { return tetromino_generator_->Get(tetromino_); }

  virtual void Render() const override {
    TextPane::Render();

    if (Tetromino::Type::Empty == tetromino_) {
      return;
    }
    assets_->GetTetromino(tetromino_)->Render(x_ + 15, y_ + 50 + txt_height_);
  }

 private:
  bool wait_for_lock_ = false;
  Tetromino::Type tetromino_ = Tetromino::Type::Empty;

  const std::shared_ptr<TetrominoGenerator> tetromino_generator_;
};
