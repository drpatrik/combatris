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
  Animation(SDL_Renderer *renderer,  const std::shared_ptr<Assets>& assets) : renderer_(renderer), assets_(assets) {}

  Animation(const Animation&) = delete;

  virtual ~Animation() noexcept = default;

  virtual void Render(double) = 0;

  virtual std::pair<bool, Event::Type> IsReady() const = 0;

  operator SDL_Renderer *() const { return renderer_; }

  const Assets& GetAsset() const { return *assets_; }

  virtual std::string name() const { return typeid(*this).name(); }

protected:
  double x_ = 0.0;
  double y_ = 0.0;

  void RenderCopy(SDL_Texture *texture, const SDL_Rect& rc) { SDL_RenderCopy(*this, texture, nullptr, &rc); }

private:
  SDL_Renderer* renderer_;
  std::shared_ptr<Assets> assets_ = nullptr;
};

class ScoreAnimation final : public Animation {
 public:
  ScoreAnimation(SDL_Renderer* renderer,  const std::shared_ptr<Assets>& assets,  const Position& pos, int score)
      : Animation(renderer, assets) {
    int width, height;
    std::tie(texture_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(Bold30), std::to_string(score), Color::Coral);

    auto x = col_to_pixel_adjusted(pos.col()) + Center(kMinoWidth * 4, width);
    auto y = row_to_pixel_adjusted(pos.row());

    if (x + width > kMatrixEndX) {
      x = kMatrixEndX - width;
    } else if (x < kMatrixStartX) {
      x = kMatrixStartX;
    }
    if (y + height > kMatrixEndY) {
      y = kMatrixEndY - height;
    }
    rc_ = { x, y, width, height };
    y_ = rc_.y;
    end_pos_ = y_ - (kMinoHeight * 2);
  }

  virtual void Render(double delta) override {
    const double kIncY = delta * 75.0;

    rc_.y = static_cast<int>(y_);
    RenderCopy(texture_.get(), rc_);
    y_ -= kIncY;
  }

  virtual std::pair<bool, Event::Type> IsReady() const override { return std::make_pair(y_ <= end_pos_, Event::Type::None); }

 private:
  SDL_Rect rc_;
  UniqueTexturePtr texture_ = nullptr;
  double end_pos_;
};

class LinesClearedAnimation final : public Animation {
 public:
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

      for (size_t l = kVisibleColStart; l < kVisibleColEnd; ++l) {
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
    RenderCopy(texture_.get(), rc_);
    ticks_ += delta;
    if (ticks_ >= 1.0) {
      countdown_--;
      CreateTexture(countdown_);
      ticks_ = 0.0;
    }
  }

  virtual std::pair<bool, Event::Type> IsReady() const override { return std::make_pair(countdown_ - 1 < 0.0, type_); }

  void CreateTexture(int i) {
    int width, height;

    std::tie(texture_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(Normal200), std::to_string(i), Color::White);
    rc_ = { kMatrixStartX + Center(kMatrixWidth, width), kMatrixStartY + 100, width, height };
  }

 private:
  Event::Type type_;
  int countdown_;
  double ticks_ = 0.0;
  SDL_Rect rc_;
  UniqueTexturePtr texture_;
};

class MessageAnimation final : public Animation {
 public:
  MessageAnimation(SDL_Renderer *renderer, const std::shared_ptr<Assets>& assets, const std::string& msg, Color color, double display_speed = 45.0)
      : Animation(renderer, assets), display_speed_(display_speed) {
    int width, height;

    std::tie(texture_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(Bold55), msg, color);

    rc_ = { kMatrixStartX + Center(kMatrixWidth, width), kMatrixStartY + Center(kMatrixHeight, height), width, height };
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

  virtual ~OnFloorAnimation() { SDL_SetTextureAlphaMod(alpha_texture_.get(), alpha_saved_); }

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
    int width, height;

    std::tie(texture_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(Normal45), "Paused ... ", Color::White);

    rc_ = { kMatrixStartX + Center(kMatrixWidth, width), kMatrixStartY + Center(kMatrixHeight, height) , width, height };
  }

  virtual void Render(double) override {
    SetBlackBackground(*this);
    RenderCopy(texture_.get(), rc_);
  }

  virtual std::pair<bool, Event::Type> IsReady() const override { return std::make_pair(unpause_pressed_, Event::Type::UnPause); }

private:
  bool& unpause_pressed_;
  UniqueTexturePtr texture_;
  SDL_Rect rc_;
};

class SplashScreenAnimation final : public Animation {
 public:
  SplashScreenAnimation(SDL_Renderer *renderer, const std::shared_ptr<CombatrisMenu>& menu, const std::shared_ptr<Assets>& assets)
      : Animation(renderer, assets), menu_view_(renderer, { kMatrixStartX, 0, kMatrixWidth, kMenuHeight }, assets->fonts(), menu, menu.get()) {
    int width, height;

    std::tie(texture_1_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(Bold55), "COMBATRIS", Color::SteelGray);
    rc_1_ = { kMatrixStartX + Center(kMatrixWidth, width), kMatrixStartY + 100, width, height };
    menu_view_.SetY(rc_1_.y + height + 25);
    std::tie(texture_2_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(ObelixPro18), "Press 'N' or Start to play", Color::White);
    rc_2_ = { kMatrixStartX + Center(kMatrixWidth, width), rc_1_.y + kMenuHeight, width, height };
  }

  virtual void Render(double) override {
    SetBlackBackground(*this);
    RenderCopy(texture_1_.get(), rc_1_);
    RenderCopy(texture_2_.get(), rc_2_);
    menu_view_.Render();
  }

  virtual std::pair<bool, Event::Type> IsReady() const override { return std::make_pair(false, Event::Type::None); }

private:
  UniqueTexturePtr texture_1_;
  UniqueTexturePtr texture_2_;
  UniqueTexturePtr texture_3_;
  SDL_Rect rc_1_;
  SDL_Rect rc_2_;
  MenuView menu_view_;
};

class GameOverAnimation final : public Animation {
 public:
  GameOverAnimation(SDL_Renderer *renderer, const std::shared_ptr<CombatrisMenu>& menu, const std::shared_ptr<Assets>& assets)
      : Animation(renderer, assets), menu_view_(renderer, { kMatrixStartX, 0, kMatrixWidth, kMenuHeight }, assets->fonts(), menu, menu.get()) {
    int width, height;

    std::tie(texture_1_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(Normal55), "Game Over", Color::White);
    rc_1_ = { kMatrixStartX + Center(kMatrixWidth, width), kMatrixStartY + 100 , width, height };
    menu_view_.SetY(rc_1_.y + height + 25);

    std::tie(texture_2_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(Normal25), "Press 'N' or Start to play", Color::White);
    rc_2_ = { kMatrixStartX + Center(kMatrixWidth, width), rc_1_.y + kMenuHeight, width, height };

    blackbox_rc_ = { rc_1_.x - 20, rc_1_.y - 20, rc_1_.w + 40, rc_1_.h + kMenuHeight };
  }

  virtual void Render(double) override {
    SDL_SetRenderDrawColor(*this, 0, 0, 0, 0);
    SDL_RenderFillRect(*this, &blackbox_rc_);
    RenderCopy(texture_1_.get(), rc_1_);
    RenderCopy(texture_2_.get(), rc_2_);
    RenderCopy(texture_3_.get(), rc_3_);
    menu_view_.Render();
  }

  virtual std::pair<bool, Event::Type> IsReady() const override { return std::make_pair(false, Event::Type::None); }

private:
  UniqueTexturePtr texture_1_;
  UniqueTexturePtr texture_2_;
  UniqueTexturePtr texture_3_;
  SDL_Rect rc_1_;
  SDL_Rect rc_2_;
  SDL_Rect rc_3_;
  SDL_Rect blackbox_rc_;
  MenuView menu_view_;
};

// We make this class generic when we have more gifs
class HourglassAnimation final : public Animation {
 public:
  HourglassAnimation(SDL_Renderer* renderer, const std::shared_ptr<Assets>& assets, const std::shared_ptr<MultiPlayer>& multi_player)
      : Animation(renderer, assets), multi_player_(multi_player), textures_(GetAsset().GetHourGlassTextures()) {
    int width, height;

    std::tie(text_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(Normal25), "Waiting for all players", Color::White);
    rc_ = { kMatrixStartX + Center(kMatrixWidth, width), kMatrixStartY + 100 , width, height };
  }

  virtual void Render(double delta) override {
    const SDL_Rect kHourglassRc { kMatrixStartX + Center(kMatrixWidth, 128), kMatrixStartY + 150, 128, 128 };

    SetBlackBackground(*this);
    RenderCopy(textures_.at(frame_).get(), kHourglassRc);
    RenderCopy(text_.get(), rc_);
    ticks_ += delta;
    if (ticks_ >= 0.07) {
      frame_++;
      if (frame_ == textures_.size()) {
        frame_ = 0;
      }
      ticks_ = 0.0;
    }
  }

  virtual std::pair<bool, Event::Type> IsReady() const override {
    return std::make_pair(multi_player_->CanStartGame(), Event::Type::NextTetromino);
  }

 private:
  size_t frame_ = 0;
  double ticks_ = 0.0;
  UniqueTexturePtr text_;
  SDL_Rect rc_;
  std::shared_ptr<MultiPlayer> multi_player_;
  std::vector<std::shared_ptr<SDL_Texture>> textures_;
};
