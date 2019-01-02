#pragma once

#include "game/events.h"
#include "game/panes/pane.h"

class Knockout final : public Pane, public EventListener {
 public:
  static constexpr double kDisplayTime = 0.30;
  static const int kCircleDim = 100;
  static const int kX = kMatrixStartX - kMinoWidth - (kCircleDim + kSpace);
  static const int kY = kMatrixStartY + 300;

  using Texture = utility::Texture;

  Knockout(SDL_Renderer *renderer, const std::shared_ptr<Assets> &assets)
      : Pane(renderer, kX, kY, assets) {
    caption_texture_.SetXY(kX + 15, kY - 20);
    circle_texture_.SetX(kX);
    circle_texture_.SetY(kY);
    caption_texture_.SetX(kX + 15);
    caption_texture_.SetY(kY - 20);
    plus_one_texture_.SetX(circle_texture_.x() + utility::Center(kCircleDim, plus_one_texture_.width()));
    plus_one_texture_.SetY(circle_texture_.y() + utility::Center(kCircleDim, plus_one_texture_.height()));
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
    Pane::RenderCopy(circle_texture_);
    Pane::RenderCopy(caption_texture_);
    if (show_plus_one_)  {
      Pane::RenderCopy(plus_one_texture_);
      ticks_ += delta_time;
      show_plus_one_ = (ticks_ < kDisplayTime);
    } else if (n_ko_ > 0) {
      Pane::RenderCopy(n_ko_texture_);
    }
  }

  virtual void Reset() override { n_ko_ = 0; }

 protected:
  void Display(int ko) {
    n_ko_texture_ = Texture(renderer_, assets_->GetFont(ObelixPro40), std::to_string(ko), Color::Yellow);
    n_ko_texture_.SetX(circle_texture_.x() + utility::Center(kCircleDim, n_ko_texture_.width()));
    n_ko_texture_.SetY(circle_texture_.y() + utility::Center(kCircleDim, n_ko_texture_.height()));
  }

 private:
  double ticks_ = 0;
  bool show_plus_one_ = false;
  Texture circle_texture_ = Texture(assets_->GetTexture(Assets::Type::Circle));
  Texture caption_texture_ = Texture(renderer_, assets_->GetFont(ObelixPro35), "K.O.", Color::White);
  Texture plus_one_texture_ = Texture(renderer_, assets_->GetFont(ObelixPro50), "+1", Color::Blue);
  Texture n_ko_texture_;
  int n_ko_ = 0;
};
