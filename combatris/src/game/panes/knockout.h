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
    caption_rc_ = { kX + 20, kY - 20, 0, 0 };
    std::tie(caption_texture_, caption_rc_.w, caption_rc_.h) = CreateTextureFromText(renderer_, assets_->GetFont(KoFont), "K.O.", Color::White);
  }

  virtual void Update(const Event& event) override {
    if (!event.Is(Event::Type::BattleYouDidKO)) {
      return;
    }
    n_ko_ += event.value_;
    Display(n_ko_);
  }

  virtual void Render(double) override {
    if (0 == n_ko_) {
      return;
    }
    Pane::RenderCopy(circle_texture_.get(), circle_rc_);
    Pane::RenderCopy(caption_texture_.get(), caption_rc_);
    Pane::RenderCopy(n_ko_texture_.get(), n_ko_rc_);
  }

  virtual void Reset() override { n_ko_ = 0; }

 protected:
  void Display(int ko) {
    std::tie(n_ko_texture_, n_ko_rc_.w, n_ko_rc_.h) = CreateTextureFromText(renderer_, assets_->GetFont(TimerFont), std::to_string(ko), Color::Yellow);
    n_ko_rc_.x = circle_rc_.x + (kCircleDim - n_ko_rc_.w) / 2;
    n_ko_rc_.y = circle_rc_.y + (kCircleDim - n_ko_rc_.h) / 2;
}

 private:
  SDL_Rect circle_rc_;
  std::shared_ptr<SDL_Texture> circle_texture_;
  SDL_Rect caption_rc_;
  std::shared_ptr<SDL_Texture> caption_texture_;
  SDL_Rect n_ko_rc_;
  std::shared_ptr<SDL_Texture> n_ko_texture_;
  int n_ko_ = 0;
};
