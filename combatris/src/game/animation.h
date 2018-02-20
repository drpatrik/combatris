#pragma once

#include "game/tetromino_sprite.h"
#include "game/assets.h"

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

  virtual std::pair<bool, Event::Type> IsReady()  = 0;

  operator SDL_Renderer *() const { return renderer_; }

  const Assets& GetAsset() const { return *assets_; }

  virtual std::string name() const { return typeid(*this).name(); }

protected:
  double x_ = 0.0;
  double y_ = 0.0;

  void RenderCopy(SDL_Texture *texture, const SDL_Rect &rc) { SDL_RenderCopy(*this, texture, nullptr, &rc); }

private:
  SDL_Renderer* renderer_;
  std::shared_ptr<Assets> assets_ = nullptr;
};

class ScoreAnimation final : public Animation {
 public:
  ScoreAnimation(SDL_Renderer* renderer,  const std::shared_ptr<Assets>& assets,  const Position& pos, int score)
      : Animation(renderer, assets) {
    int width, height;
    std::tie(texture_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(Bold25), std::to_string(score), Color::White);

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
    rc_.y = static_cast<int>(y_);
    RenderCopy(texture_.get(), rc_);
    y_ -= delta * 65.0;
  }

  virtual std::pair<bool, Event::Type> IsReady() override { return std::make_pair(y_ <= end_pos_, Event::Type::None); }

 private:
  SDL_Rect rc_;
  UniqueTexturePtr texture_ = nullptr;
  double end_pos_;
};

class ClearedLinesAnimation final : public Animation {
public:
  ClearedLinesAnimation(SDL_Renderer *renderer,const std::shared_ptr<Assets> &assets, const Lines& lines)
      : Animation(renderer, assets), lines_(lines) {}

  virtual void Render(double delta) override {
    const double kIncY = delta * 350;
    y_ += kIncY;
  }

  virtual std::pair<bool, Event::Type> IsReady() override { return std::make_pair(true, Event::Type::None); }

private:
  Lines lines_;
};

class CountDownAnimation final : public Animation {
 public:
  CountDownAnimation(SDL_Renderer *renderer, const std::shared_ptr<Assets> &assets, Event::Type type)
      : Animation(renderer, assets), type_(type) {
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

  virtual  std::pair<bool, Event::Type> IsReady() override { return std::make_pair(countdown_ - 1 < 0.0, type_); }

  void CreateTexture(int i) {
    int width, height;

    std::tie(texture_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(Normal200), std::to_string(i), Color::White);
    rc_ = { kMatrixStartX + Center(kMatrixWidth, width), kMatrixStartY + 100, width, height };
  }

private:
  Event::Type type_;
  int countdown_ = 3;
  double ticks_ = 0.0;
  SDL_Rect rc_;
  UniqueTexturePtr texture_;
  };

class LevelUpAnimation final : public Animation {
 public:
  LevelUpAnimation(SDL_Renderer *renderer, std::shared_ptr<Assets> &assets)
      : Animation(renderer, assets) {
    int width, height;

    std::tie(texture_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(Bold55), "LEVEL UP", Color::SteelGray);

    rc_ = { kMatrixStartX + Center(kMatrixWidth, width), kMatrixStartY + Center(kMatrixHeight, height), width, height };
    y_ = rc_.y;
    end_pos_ = y_ - (kMinoHeight * 3);
  }

  virtual void Render(double delta) override {
    SDL_SetTextureAlphaMod(texture_.get(), static_cast<Uint8>(alpha_));
    rc_.y = static_cast<int>(y_);
    RenderCopy(texture_.get(), rc_);

    alpha_ -= delta * 150.0;
    y_ -= delta * 65.0;
  }

  virtual std::pair<bool, Event::Type> IsReady() override { return std::make_pair(y_ <= end_pos_, Event::Type::None); }

private:
  double alpha_ = 255.0;
  UniqueTexturePtr texture_;
  SDL_Rect rc_;
  double end_pos_;
};

class FloorReachedAnimation final : public Animation {
 public:
  FloorReachedAnimation(SDL_Renderer *renderer, std::shared_ptr<Assets> &assets, const std::shared_ptr<TetrominoSprite>& tetromino_sprite)
      : Animation(renderer, assets), tetromino_sprite_(tetromino_sprite), tetromino_(tetromino_sprite->tetromino()) {
    alpha_texture_ = assets->GetAlphaTextures(tetromino_sprite_->type());
    SDL_GetTextureAlphaMod(alpha_texture_.get(), &alpha_saved_);
    SDL_SetTextureAlphaMod(alpha_texture_.get(), static_cast<Uint8>(alpha_));
  }

  virtual ~FloorReachedAnimation() { SDL_SetTextureAlphaMod(alpha_texture_.get(), alpha_saved_); }

  virtual void Render(double) override {
    const auto& pos = tetromino_sprite_->pos();

    SDL_RenderSetClipRect(*this, &kMatrixRc);
    tetromino_.Render(pos.x(), pos.y(), alpha_texture_.get(), tetromino_sprite_->angle());
    SDL_RenderSetClipRect(*this, nullptr);
  }

  virtual std::pair<bool, Event::Type> IsReady() override {
    return std::make_pair(tetromino_sprite_->WaitForLockDelay(), Event::Type::None);
  }

private:
  Uint8 alpha_ = 150.0;
  Uint8 alpha_saved_;
  std::shared_ptr<SDL_Texture> alpha_texture_;
  std::shared_ptr<TetrominoSprite> tetromino_sprite_;
  const Tetromino& tetromino_;
};

class PauseAnimation final : public Animation {
 public:
  PauseAnimation(SDL_Renderer *renderer, std::shared_ptr<Assets> &assets, bool &unpause_pressed)
      : Animation(renderer, assets), unpause_pressed_(unpause_pressed) {
    int width, height;

    std::tie(texture_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(Normal45), "Paused ... ", Color::White);

    rc_ = { kMatrixStartX + Center(kMatrixWidth, width), kMatrixStartY + Center(kMatrixHeight, height) , width, height };
  }

  virtual void Render(double) override {
    SetBlackBackground(*this);
    RenderCopy(texture_.get(), rc_);
  }

  virtual std::pair<bool, Event::Type> IsReady() override { return std::make_pair(unpause_pressed_, Event::Type::UnPause); }

private:
  bool& unpause_pressed_;
  UniqueTexturePtr texture_;
  SDL_Rect rc_;
};

class SplashScreenAnimation final : public Animation {
 public:
  SplashScreenAnimation(SDL_Renderer *renderer, std::shared_ptr<Assets> &assets)
      : Animation(renderer, assets) {
    int width, height;

    std::tie(texture_1_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(Normal55), "COMBATRIS", Color::SteelGray);
    rc_1_ = { kMatrixStartX + Center(kMatrixWidth, width), kMatrixStartY + 100, width, height };
    std::tie(texture_2_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(Normal25), "Press 'N' or Start to play", Color::White);
    rc_2_ = { kMatrixStartX + Center(kMatrixWidth, width), rc_1_.y + rc_1_.h + 10 , width, height };
  }

  virtual void Render(double) override {
    SetBlackBackground(*this);
    RenderCopy(texture_1_.get(), rc_1_);
    RenderCopy(texture_2_.get(), rc_2_);
  }

  virtual std::pair<bool, Event::Type> IsReady() override { return std::make_pair(false, Event::Type::None); }

private:
  UniqueTexturePtr texture_1_;
  UniqueTexturePtr texture_2_;
  SDL_Rect rc_1_;
  SDL_Rect rc_2_;
};

class GameOverAnimation final : public Animation {
 public:
  GameOverAnimation(SDL_Renderer *renderer, std::shared_ptr<Assets> &assets)
      : Animation(renderer, assets) {
    int width, height;

    std::tie(texture_1_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(Normal55), "Game Over", Color::White);
    rc_1_ = { kMatrixStartX + Center(kMatrixWidth, width), kMatrixStartY + 100 , width, height };
    std::tie(texture_2_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(Normal25), "Press 'N' or Start to play", Color::White);
    rc_2_ = { kMatrixStartX + Center(kMatrixWidth, width), rc_1_.y + rc_1_.h + 10 , width, height };
    blackbox_rc_ = { rc_1_.x - 20, rc_1_.y - 20, rc_1_.w + 40, rc_1_.h + rc_2_.h + 50 };
  }

  virtual void Render(double) override {
    SDL_SetRenderDrawColor(*this, 0, 0, 0, 0);
    SDL_RenderFillRect(*this, &blackbox_rc_);
    RenderCopy(texture_1_.get(), rc_1_);
    RenderCopy(texture_2_.get(), rc_2_);
  }

  virtual std::pair<bool, Event::Type> IsReady() override { return std::make_pair(false, Event::Type::None); }

private:
  UniqueTexturePtr texture_1_;
  UniqueTexturePtr texture_2_;
  SDL_Rect rc_1_;
  SDL_Rect rc_2_;
  SDL_Rect blackbox_rc_;
};
