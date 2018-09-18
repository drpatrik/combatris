#pragma once

#include "game/events.h"
#include "game/panes/pane.h"

class Knockout final : public Pane, public EventListener {
 public:
  static constexpr double kDisplayTime = 0.30;
  static const int kCircleDim = 100;
  static const int kX = kMatrixStartX - kMinoWidth - (kCircleDim + kSpace);
  static const int kY = kMatrixStartY + 300;

  Knockout(SDL_Renderer *renderer, const std::shared_ptr<Assets> &assets)
      : Pane(renderer, kX, kY, assets) {
    std::tie(circle_texture_, circle_rc_.w, circle_rc_.h) = assets_->GetTexture(Assets::Type::Circle);
    circle_rc_ = { kX, kY, kCircleDim, kCircleDim };
    caption_rc_ = { kX + 15, kY - 20, 0, 0 };
    std::tie(caption_texture_, caption_rc_.w, caption_rc_.h) =
        CreateTextureFromText(renderer_, assets_->GetFont(ObelixPro35), "K.O.", Color::White);
    std::tie(plus_one_texture_, plus_one_rc_.w, plus_one_rc_.h) =
        CreateTextureFromText(renderer_, assets_->GetFont(ObelixPro50), "+1", Color::Blue);
    plus_one_rc_.x = circle_rc_.x + utility::Center(kCircleDim, plus_one_rc_.w);
    plus_one_rc_.y = circle_rc_.y + utility::Center(kCircleDim, plus_one_rc_.h);
  }

  virtual void Update(const Event& event) override {
    if (!event.Is(Event::Type::BattleYouDidKO)) {
      return;
    }
    n_ko_ += 1;
    Display(n_ko_);
    ticks_ = 0.0;
    show_plus_one_ = true;
  }

  virtual void Render(double delta_time) override {
    if (0 == n_ko_) {
      return;
    }
    Pane::RenderCopy(circle_texture_.get(), circle_rc_);
    Pane::RenderCopy(caption_texture_.get(), caption_rc_);
    if (show_plus_one_)  {
      Pane::RenderCopy(plus_one_texture_.get(), plus_one_rc_);
      ticks_ += delta_time;
      show_plus_one_ = (ticks_ < kDisplayTime);
    } else {
      Pane::RenderCopy(n_ko_texture_.get(), n_ko_rc_);
    }
  }

  virtual void Reset() override { n_ko_ = 0; }

 protected:
  void Display(int ko) {
    std::tie(n_ko_texture_, n_ko_rc_.w, n_ko_rc_.h) = CreateTextureFromText(renderer_, assets_->GetFont(ObelixPro40), std::to_string(ko), Color::Yellow);
    n_ko_rc_.x = circle_rc_.x + utility::Center(kCircleDim, n_ko_rc_.w);
    n_ko_rc_.y = circle_rc_.y + utility::Center(kCircleDim, n_ko_rc_.h);
}

 private:
  double ticks_ = 0;
  bool show_plus_one_ = false;
  SDL_Rect circle_rc_;
  std::shared_ptr<SDL_Texture> circle_texture_;
  SDL_Rect caption_rc_;
  utility::UniqueTexturePtr caption_texture_;
  SDL_Rect plus_one_rc_;
  utility::UniqueTexturePtr plus_one_texture_;
  SDL_Rect n_ko_rc_;
  utility::UniqueTexturePtr n_ko_texture_;
  int n_ko_ = 0;
};
