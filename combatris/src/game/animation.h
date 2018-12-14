#pragma once

#include "utility/menu_view.h"
#include "game/panes/multi_player.h"
#include "game/tetromino_sprite.h"
#include "game/combatris_menu.h"

namespace {

const SDL_Rect kMatrixRc = { kMatrixStartX, kMatrixStartY, kMatrixWidth, kMatrixHeight };

void SetBlackBackground(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderFillRect(renderer, &kMatrixRc);
}

} // namespace

class Animation {
public:
  using Color = utility::Color;
  using Texture = utility::Texture;
  using UniqueTexturePtr = utility::UniqueTexturePtr;

  Animation(SDL_Renderer *renderer,  const std::shared_ptr<Assets>& assets) : renderer_(renderer), assets_(assets) {}

  Animation(const Animation&) = delete;

  virtual ~Animation() noexcept = default;

  virtual void Render(double) = 0;

  virtual std::pair<bool, Event::Type> IsReady() const = 0;

  inline operator SDL_Renderer*() const { return renderer_; }

  inline const Assets& GetAsset() const { return *assets_; }

  virtual std::string name() const { return typeid(*this).name(); }

protected:
  double x_ = 0.0;
  double y_ = 0.0;

  inline void RenderCopy(SDL_Texture *texture, const SDL_Rect& rc) { SDL_RenderCopy(*this, texture, nullptr, &rc); }

  inline void RenderCopy(Texture& texture) { SDL_RenderCopy(*this, texture, nullptr, texture); }

private:
  SDL_Renderer* renderer_;
  std::shared_ptr<Assets> assets_ = nullptr;
};

class ScoreAnimation final : public Animation {
 public:
  ScoreAnimation(SDL_Renderer* renderer,  const std::shared_ptr<Assets>& assets,  const Position& pos, int score)
      : Animation(renderer, assets) {
    texture_ = Texture(*this, GetAsset().GetFont(Bold30), std::to_string(score), Color::Coral);

    auto x = col_to_pixel_adjusted(pos.col()) + utility::Center(kMinoWidth * 4, texture_.width());
    auto y = row_to_pixel_adjusted(pos.row());

    if (x + texture_.width() > kMatrixEndX) {
      x = kMatrixEndX - texture_.width();
    } else if (x < kMatrixStartX) {
      x = kMatrixStartX;
    }
    if (y + texture_.height() > kMatrixEndY) {
      y = kMatrixEndY - texture_.height();
    }
    texture_.SetXY(x, y);
    y_ = y;
    end_pos_ = y_ - (kMinoHeight * 2);
  }

  virtual void Render(double delta) override {
    const double kIncY = delta * 75.0;

    texture_.SetY(static_cast<int>(y_));
    RenderCopy(texture_);
    y_ -= kIncY;
  }

  virtual std::pair<bool, Event::Type> IsReady() const override { return std::make_pair(y_ <= end_pos_, Event::Type::None); }

 private:
  Texture texture_;
  double end_pos_;
};

class LinesClearedAnimation final : public Animation {
 public:
  const int kRows = kMatrixLastRow + 3;

  LinesClearedAnimation(SDL_Renderer *renderer, const std::shared_ptr<Assets>& assets, const Lines& lines)
      : Animation(renderer, assets), lines_(lines) {
    end_pos_ = ((kRows - lines.at(0).row_) + lines.size() + 1.5) * kMinoHeight;
  }

  virtual void Render(double delta) override {
    const double kIncY = delta * 550.0;
    const double direction = (abs_y_ < kMinoHeight) ? -1 : 1;

    for (const auto& line : lines_) {
      auto y = row_to_pixel_adjusted(line.row_) + y_;

      const auto& minos = line.minos_;

      for (int l = kMatrixFirstCol; l < kMatrixLastCol; ++l) {
        auto x = col_to_pixel_adjusted(l);

        const auto& tetromino = GetAsset().GetTetromino(static_cast<Tetromino::Type>(minos[l]));

        tetromino->Render(x, static_cast<int>(y));
      }
    }
    y_ += (kIncY * direction);
    abs_y_ += kIncY;
  }

  virtual std::pair<bool, Event::Type> IsReady() const override { return std::make_pair(abs_y_ >= end_pos_, Event::Type::None); }

 private:
  double abs_y_ = 0.0;
  Lines lines_;
  double end_pos_;
};

class CountDownAnimation final : public Animation {
 public:
  CountDownAnimation(SDL_Renderer *renderer, const std::shared_ptr<Assets>& assets, int countdown, Event::Type type)
      : Animation(renderer, assets), type_(type), countdown_(countdown) {
    CreateTexture(countdown_);
  }

  virtual void Render(double delta) override {
    SetBlackBackground(*this);
    RenderCopy(texture_);
    ticks_ += delta;
    if (ticks_ >= 1.0) {
      countdown_--;
      CreateTexture(countdown_);
      ticks_ = 0.0;
    }
  }

  virtual std::pair<bool, Event::Type> IsReady() const override { return std::make_pair(countdown_ - 1 < 0.0, type_); }

  void CreateTexture(int i) {
    texture_ = Texture(*this, GetAsset().GetFont(Normal200), std::to_string(i), Color::White);
    texture_.SetXY(kMatrixStartX + utility::Center(kMatrixWidth, texture_.width()), kMatrixStartY + 100);
  }

 private:
  Event::Type type_;
  int countdown_;
  double ticks_ = 0.0;
  Texture texture_;
};

class MessageAnimation final : public Animation {
 public:
  MessageAnimation(SDL_Renderer *renderer, const std::shared_ptr<Assets>& assets, const std::string& msg, Color color, double display_speed = 45.0)
      : Animation(renderer, assets), display_speed_(display_speed) {
    int width, height;

    std::tie(texture_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(Bold55), msg, color);

    rc_ = { kMatrixStartX + utility::Center(kMatrixWidth, width), kMatrixStartY + utility::Center(kMatrixHeight, height), width, height };
    y_ = rc_.y;
    end_pos_ = y_ - (kMinoHeight * 3);
  }

  virtual void Render(double delta) override {
    SDL_SetTextureAlphaMod(texture_.get(), static_cast<Uint8>(alpha_));
    rc_.y = static_cast<int>(y_);
    RenderCopy(texture_.get(), rc_);

    alpha_ -= delta * 100.0;
    y_ -= delta * display_speed_;
  }

  virtual std::pair<bool, Event::Type> IsReady() const override { return std::make_pair(y_ <= end_pos_, Event::Type::None); }

private:
  double alpha_ = 255.0;
  double display_speed_;
  UniqueTexturePtr texture_;
  SDL_Rect rc_;
  double end_pos_;
};

class OnFloorAnimation final : public Animation {
 public:
  OnFloorAnimation(SDL_Renderer *renderer, const std::shared_ptr<Assets>& assets, const std::shared_ptr<TetrominoSprite>& tetromino_sprite)
      : Animation(renderer, assets), tetromino_sprite_(tetromino_sprite), tetromino_(tetromino_sprite->tetromino()) {
    alpha_texture_ = assets->GetAlphaTextures(tetromino_sprite_->tetromino().type());
    SDL_GetTextureAlphaMod(alpha_texture_.get(), &alpha_saved_);
    SDL_SetTextureAlphaMod(alpha_texture_.get(), static_cast<Uint8>(kAlpha));
  }

  virtual ~OnFloorAnimation() noexcept { SDL_SetTextureAlphaMod(alpha_texture_.get(), alpha_saved_); }

  virtual void Render(double) override {
    SDL_RenderSetClipRect(*this, &kMatrixRc);
    tetromino_sprite_->Render(alpha_texture_);
    SDL_RenderSetClipRect(*this, nullptr);
  }

  virtual std::pair<bool, Event::Type> IsReady() const override {
    return std::make_pair(tetromino_sprite_->WaitForLockDelay(), Event::Type::None);
  }

private:
  const Uint8 kAlpha = 150;
  Uint8 alpha_saved_;
  std::shared_ptr<SDL_Texture> alpha_texture_;
  std::shared_ptr<TetrominoSprite> tetromino_sprite_;
  const Tetromino& tetromino_;
};

class PauseAnimation final : public Animation {
 public:
  PauseAnimation(SDL_Renderer *renderer, const std::shared_ptr<Assets>& assets, bool& unpause_pressed)
      : Animation(renderer, assets), unpause_pressed_(unpause_pressed) {
    texture_ = Texture(*this, GetAsset().GetFont(Normal45), "Paused ... ", Color::White);
    texture_.SetXY(kMatrixStartX + utility::Center(kMatrixWidth, texture_.width()), kMatrixStartY + utility::Center(kMatrixHeight, texture_.height()));
  }

  virtual void Render(double) override {
    SetBlackBackground(*this);
    RenderCopy(texture_);
  }

  virtual std::pair<bool, Event::Type> IsReady() const override { return std::make_pair(unpause_pressed_, Event::Type::UnPause); }

private:
  bool& unpause_pressed_;
  Texture texture_;
};

class SplashScreenAnimation final : public Animation {
 public:
  SplashScreenAnimation(SDL_Renderer *renderer, const std::shared_ptr<CombatrisMenu>& menu, const std::shared_ptr<Assets>& assets)
      : Animation(renderer, assets), menu_view_(renderer, { kMatrixStartX, 0, kMatrixWidth, kMenuHeight }, assets->fonts(), menu, menu.get()) {

    texture_1_.SetXY(kMatrixStartX + utility::Center(kMatrixWidth, texture_1_.width()), kMatrixStartY + 100);
    menu_view_.SetY(texture_1_.y() + texture_1_.height() + 25);
    texture_2_.SetXY(kMatrixStartX + utility::Center(kMatrixWidth, texture_2_.width()), texture_1_.y() + kMenuHeight);

  }

  virtual void Render(double) override {
    SetBlackBackground(*this);
    RenderCopy(texture_1_);
    RenderCopy(texture_2_);
    menu_view_.Render();
  }

  virtual std::pair<bool, Event::Type> IsReady() const override { return std::make_pair(false, Event::Type::None); }

private:
  Texture texture_1_ = Texture(*this, GetAsset().GetFont(Bold55), "COMBATRIS", Color::SteelGray);
  Texture texture_2_ = Texture(*this, GetAsset().GetFont(ObelixPro18), "Press N or START to play", Color::White);
  utility::MenuView menu_view_;
};

class GameOverAnimation final : public Animation {
 public:
  GameOverAnimation(SDL_Renderer* renderer, const std::shared_ptr<CombatrisMenu>& menu,
                    const std::shared_ptr<Assets>& assets, const std::string text = "Game Over")
      : Animation(renderer, assets),
        menu_view_(renderer, {kMatrixStartX, 0, kMatrixWidth, kMenuHeight}, assets->fonts(), menu, menu.get()), text_(text) {

    texture_1_ = Texture(*this, GetAsset().GetFont(Normal55), text, Color::White);
    texture_1_.SetXY(kMatrixStartX + utility::Center(kMatrixWidth, texture_1_.width()), kMatrixStartY + 100);
    menu_view_.SetY(texture_1_.y() + texture_1_.height() + 25);
    texture_2_.SetXY(kMatrixStartX + utility::Center(kMatrixWidth, texture_2_.width()), texture_1_.y() + kMenuHeight);

    blackbox_rc_ = { texture_2_.x() - 20, texture_1_.y() - 20, texture_2_.width() + 40, texture_1_.height() + kMenuHeight };
  }

  virtual void Render(double) override {
    SDL_SetRenderDrawColor(*this, 0, 0, 0, 0);
    SDL_RenderFillRect(*this, &blackbox_rc_);
    RenderCopy(texture_1_);
    RenderCopy(texture_2_);
    menu_view_.Render();
  }

  virtual std::pair<bool, Event::Type> IsReady() const override { return std::make_pair(false, Event::Type::None); }

private:
  Texture texture_1_;
  Texture texture_2_ = Texture(*this, GetAsset().GetFont(ObelixPro18), "Press N or START to play", Color::White);
  SDL_Rect blackbox_rc_;
  utility::MenuView menu_view_;
  std::string text_;
};

// We make this class generic when we have more gifs
class HourglassAnimation final : public Animation {
 public:
  HourglassAnimation(SDL_Renderer* renderer, const std::shared_ptr<Assets>& assets)
      : Animation(renderer, assets), textures_(GetAsset().GetHourGlassTextures()) {
    text_ = Texture(*this, GetAsset().GetFont(Normal25), "Waiting for all players", Color::White);
    text_.SetXY(kMatrixStartX + utility::Center(kMatrixWidth, text_.width()), kMatrixStartY + 100);
  }

  virtual void Render(double delta) override {
    const SDL_Rect kHourglassRc { kMatrixStartX + utility::Center(kMatrixWidth, 128), kMatrixStartY + 150, 128, 128 };

    SetBlackBackground(*this);
    RenderCopy(textures_.at(frame_).get(), kHourglassRc);
    RenderCopy(text_);
    ticks_ += delta;
    if (ticks_ >= 0.07) {
      frame_++;
      if (textures_.size() == frame_) {
        frame_ = 0;
      }
      ticks_ = 0.0;
    }
  }

  virtual std::pair<bool, Event::Type> IsReady() const override { return std::make_pair(false, Event::Type::None); }

 private:
  size_t frame_ = 0;
  double ticks_ = 0.0;
  Texture text_;
  std::vector<std::shared_ptr<SDL_Texture>> textures_;
};
