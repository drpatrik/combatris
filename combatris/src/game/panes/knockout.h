#pragma once

#include "game/events.h"
#include "game/panes/pane.h"

class Knockout final : public Pane, public EventListener {
 public:
  static const int kCircleDim = 100;
  static const int kX = kMatrixStartX - kMinoWidth - (kCircleDim + kSpace);
  static const int kY = kMatrixStartY + 300;

  Knockout(SDL_Renderer *renderer, const std::shared_ptr<Assets> &assets)
      : Pane(renderer, kX, kY, assets) {
    std::tie(circle_texture_, circle_rc_.w, circle_rc_.h) = assets_->GetTexture(Assets::Type::Circle);
    circle_rc_ = { kX, kY, kCircleDim, kCircleDim };
    caption_rc_ = { kX + 15, kY - 25, 0, 0 };
    std::tie(caption_texture_, caption_rc_.w, caption_rc_.h) = CreateTextureFromText(renderer_, assets_->GetFont(KoFont), "K.O.", Color::Red);

  }

  virtual void Update(const Event& event) override {
    if (!event.Is(Event::Type::BattleYouDidKO)) {
      return;
    }
  }

  virtual void Render(double) override {
    Pane::RenderCopy(circle_texture_.get(), circle_rc_);
    Pane::RenderCopy(caption_texture_.get(), caption_rc_);
  }

  virtual void Reset() override {}

 private:
  SDL_Rect circle_rc_;
  std::shared_ptr<SDL_Texture> circle_texture_;
  SDL_Rect caption_rc_;
  std::shared_ptr<SDL_Texture> caption_texture_;
  //SDL_Rect n_ko_rc_;
  //std::shared_ptr<SDL_Texture> n_ko_texture_;
};
