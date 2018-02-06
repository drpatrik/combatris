#pragma once

#include "game/matrix.h"
#include "game/assets.h"

class Animation {
public:
  Animation(SDL_Renderer *renderer, Matrix& matrix, const Assets& assets)
      : renderer_(renderer), matrix_(matrix), assets_(assets) {}

  virtual ~Animation() noexcept = default;

  virtual void Start() = 0;

  virtual void Render(double) = 0;

  virtual bool IsReady() = 0;

  virtual bool Idle() const { return false; }

  virtual bool LockBoard() const { return true; }

  operator SDL_Renderer *() const { return renderer_; }

  Matrix &GetMatrix() { return matrix_; }

  const Assets& GetAsset() const { return assets_; }

protected:
  double x_ = 0.0;
  double y_ = 0.0;

  void RenderCopy(SDL_Texture *texture, const SDL_Rect &rc) {
    SDL_RenderCopy(*this, texture, nullptr, &rc);
  }

private:
  SDL_Renderer *renderer_;
  Matrix &matrix_;
  const Assets& assets_;
};

/*class ScoreAnimation final : public Animation {
 public:
  ScoreAnimation(SDL_Renderer *renderer, Grid &grid,
                 const std::vector<Position> &matches, int chains,
                 int score,
                 const std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, grid, asset_manager), score_(score), chains_(chains) {
    auto [x, y] = FindPositionForScoreAnimation(matches);

    int width, height;
    std::tie(texture_, width, height) = CreateTextureFromFramedText(*this, GetAsset().GetFont(Small), std::to_string(score_), Color::White, Color::Black);

    rc_ = { x + Center(kSpriteWidth, width), y + Center(kSpriteHeight, height), width, height };
    y_ = rc_.y;
    end_pos_ = y_ - kSpriteHeightTimes1_5;
  }

  virtual void Start() override {
    switch (chains_) {
      case 1:
        GetAudio().PlaySound(SoundEffect::RemovedOneChain);
        break;
      case 2:
        GetAudio().PlaySound(SoundEffect::RemovedTwoChains);
        break;
      default:
        GetAudio().PlaySound(SoundEffect::RemovedManyChains);
        break;
    }
  }

  virtual void Update(double delta) override {
    SDL_Rect clip_rc;
    SDL_RenderGetClipRect(*this, &clip_rc);
    SDL_RenderSetClipRect(*this, NULL);
    rc_.y = static_cast<int>(y_);
    RenderCopy(texture_.get(), rc_);
    y_ -= delta * 65.0;
    SDL_RenderSetClipRect(*this, &clip_rc);
  }

  virtual bool IsReady() override { return (y_ <= end_pos_); }

 private:
  int score_;
  int chains_;
  SDL_Rect rc_;
  UniqueTexturePtr texture_ = nullptr;
  double end_pos_;
  };*/

/*class MatchAnimation final : public Animation {
public:
  MatchAnimation(SDL_Renderer *renderer, Grid &grid,
                 const std::vector<Position> &matches, int chains,
                 const std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, grid, asset_manager), matches_(matches.begin(), matches.end()),
        score_animation_(renderer, grid, matches, chains, GetBasicScore(matches_.size()), asset_manager) {}

  virtual void Start() override {
    int i = 0;

    elements_.resize(matches_.size(), Element(SpriteID::OwnedByAnimation));
    for (const auto &m : matches_) {
      std::swap(elements_[i], GetGrid().At(m));
      i++;
    }
  }

  virtual void Update(double delta) override {
    if (!lock_board_) {
      score_animation_.Update(delta);
      return;
    }
    x_ += (75.0 * delta);
    y_ += (75.0 * delta);
    scale_w_ -= (150 * delta);
    scale_h_ -= (150 * delta);

    int i = 0;

    for (const auto &m : matches_) {
      int x = static_cast<int>(m.x() + x_);
      int y = static_cast<int>(m.y() + y_);

      SDL_Rect rc = { x, y, static_cast<int>(scale_w_), static_cast<int>(scale_h_) };
      RenderCopy(elements_[i], rc);
      i++;
    }
  }

  virtual bool IsReady() override {
    if (!lock_board_) {
      return score_animation_.IsReady();
    } else {
      if (scale_w_ <= 0.0 || scale_h_ <= 0.0) {
        for (const auto &m : matches_) {
          GetGrid().At(m) = Element(SpriteID::Empty);
        }
        lock_board_ = false;
        score_animation_.Start();
      }
    }
    return false;
  }

  virtual bool LockBoard() const override { return lock_board_; }

private:
  std::set<Position> matches_;
  std::vector<Element> elements_;
  double scale_w_ = kSpriteWidth;
  double scale_h_ = kSpriteHeight;
  bool lock_board_ = true;
  ScoreAnimation score_animation_;
  };*/

/*class MoveDownAnimation final : public Animation {
public:
  MoveDownAnimation(SDL_Renderer *renderer, Grid &grid, const Position &p,
                    const std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, grid, asset_manager), p_(p) {}

  virtual void Start() override {
    std::swap(element_, GetGrid().At(p_));
    rc_ = { p_.x(), p_.y() - kSpriteHeight, kSpriteWidth, kSpriteHeight };
    y_ = rc_.y;
  }

  virtual void Update(double delta) override {
    const double kIncY = GetGrid().IsFilling() ? delta * 500 : delta * 350;

    rc_.y = static_cast<int>(y_);
    RenderCopy(element_, rc_);
    y_ += kIncY;
  }

  virtual bool IsReady() override {
    if (y_ < p_.y()) {
      return false;
    }
    std::swap(element_, GetGrid().At(p_));
    if (p_.row() + 1 >= kRows || !GetGrid().At(p_.row() + 1, p_.col()).IsEmpty()) {
      GetAudio().PlaySound(SoundEffect::DiamondLanding);
    }
    return true;
  }

private:
  Position p_;
  SDL_Rect rc_ { 0, 0, 0, 0 };
  Element element_ = Element(OwnedByAnimation);
};

class HintAnimation final : public Animation {
public:
  HintAnimation(SDL_Renderer *renderer, Grid &grid, const Position &p1,
                const Position &p2,
                std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, grid, asset_manager), p1_(p1), p2_(p2) {}

  ~HintAnimation() {
    std::swap(e1_, GetGrid().At(p1_));
    std::swap(e2_, GetGrid().At(p2_));
  }

  virtual void Start() override {
    std::swap(e1_, GetGrid().At(p1_));
    std::swap(e2_, GetGrid().At(p2_));
    GetAudio().PlaySound(SoundEffect::Hint);
  }

  virtual void Update(double delta) override {
    const double kTwoTimesPi = 2.0 * 3.1415926535897932384626433;
    const double kRadius = kSpriteWidth / 10.0;

    angle_ += (delta * 30);
    if (angle_ > kTwoTimesPi) {
      angle_ = 0.0;
      revolutions_++;
    }
    x_ = cos(angle_) * kRadius;
    y_ = sin(angle_) * kRadius;

    e1_.Render(*this, static_cast<int>(x_) + p1_.x(), static_cast<int>(y_) + p1_.y(), true);
    e2_.Render(*this, static_cast<int>(x_) + p2_.x(), static_cast<int>(y_) + p2_.y(), true);
  }

  virtual bool IsReady() override { return (revolutions_ >= 3) ? true : false; }

  virtual bool Idle() const override { return true; }

private:
  Position p1_;
  Position p2_;
  Element e1_ = Element(SpriteID::OwnedByAnimation);
  Element e2_ = Element(SpriteID::OwnedByAnimation);
  double angle_ = 0.0;
  int revolutions_ = 0;
  };*/

/*class ExplosionAnimation final : public Animation {
public:
  ExplosionAnimation(SDL_Renderer *renderer, Grid &grid,
                     std::shared_ptr<AssetManager> &asset_manager)
      : Animation(renderer, grid, asset_manager), explosion_texture_(asset_manager->GetExplosionTextures()) {}

  virtual void Start() override {}

  virtual void Update(double delta) override {
    const SDL_Rect rc{ 100, 278, 71, 100 };

    RenderCopy(explosion_texture_.at(frame_), rc);
    animation_ticks_ += delta;
    if (animation_ticks_ >= (kTimeResolution * 5)) {
      frame_++;
      animation_ticks_ = 0.0;
    }
  }

  virtual bool IsReady() override {
    return (static_cast<size_t>(frame_) >= explosion_texture_.size());
  }

private:
  int frame_ = 0;
  double animation_ticks_ = 0.0;
  std::vector<SDL_Texture *> countdown_textures_;
  };*/

/*class LevelUpAnimation final : public Animation {
public:
  ThresholdReachedAnimation(SDL_Renderer *renderer, Grid &grid,
                            std::shared_ptr<AssetManager> &asset_manager, int value)
      : Animation(renderer, grid, asset_manager) {
    const std::string kText = std::to_string(value - (value % kThresholdMultiplier)) + " diamonds cleared";

    int width, height;

    std::tie(texture_, width, height) = CreateTextureFromText(*this, GetAsset().GetFont(Large), kText, Color::White);

    rc_ = { kBlackAreaX + Center(kBlackAreadWidth, width), kBlackAreaY + Center(kBlackAreadHeight, height) , width, height };
  }

  virtual void Start() override { GetAudio().PlaySound(SoundEffect::ThresholdReached); }

  virtual void Update(double delta) override {
    const double kFade = (ticks_ <= 0.6) ? 0.0 : 500.0;

    SDL_SetTextureAlphaMod(texture_.get(), static_cast<Uint8>(alpha_));
    RenderCopy(texture_.get(), rc_);

    alpha_ -= delta * kFade;
    ticks_ += delta;
  }

  virtual bool IsReady() override {
    if (ticks_ >= 2.0 || alpha_ <= 0) {
      return true;
    }
    return false;
  }

  virtual bool LockBoard() const override { return false; }

private:
  double alpha_ = 255.0;
  UniqueTexturePtr texture_;
  SDL_Rect rc_;
  double ticks_;
  };*/
