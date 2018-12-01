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
    caption_texture_.SetXY(kX + 15, kY - 20);
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
    if (0 == n_ko_) {
      return;
    }
    Pane::RenderCopy(circle_texture_);
    Pane::RenderCopy(caption_texture_);
    if (show_plus_one_)  {
      Pane::RenderCopy(plus_one_texture_);
      ticks_ += delta_time;
      show_plus_one_ = (ticks_ < kDisplayTime);
    } else {
      Pane::RenderCopy(n_ko_texture_);
    }
  }

  virtual void Reset() override { n_ko_ = 0; }

 protected:
  void Display(int ko) {
    n_ko_texture_ = utility::Texture(renderer_, assets_->GetFont(ObelixPro40), std::to_string(ko), Color::Yellow);
    n_ko_texture_.SetX(kX + utility::Center(kCircleDim, n_ko_texture_.width()));
    n_ko_texture_.SetY(kY + utility::Center(kCircleDim, n_ko_texture_.height()));
    n_ko_texture_.SetWidth(kCircleDim);
    n_ko_texture_.SetHeight(kCircleDim);
  }

 private:
  double ticks_ = 0;
  bool show_plus_one_ = false;
  utility::Texture circle_texture_ = utility::Texture(assets_->GetTexture(Assets::Type::Circle));
  utility::Texture caption_texture_ = utility::Texture(renderer_, assets_->GetFont(ObelixPro35), "K.O.", Color::White);
  utility::Texture plus_one_texture_ = utility::Texture(renderer_, assets_->GetFont(ObelixPro50), "+1", Color::Blue);
  utility::Texture n_ko_texture_;
  int n_ko_ = 0;
};
