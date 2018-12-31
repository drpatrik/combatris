#pragma once

#include "utility/texture.h"
#include "game/assets.h"
#include "game/panes/pane_interface.h"

class Pane : public PaneInterface {
 public:
  using Font = utility::Font;
  using Color = utility::Color;

  Pane(SDL_Renderer* renderer, int x, int y, const std::shared_ptr<Assets>& assets) : renderer_(renderer), x_(x), y_(y), assets_(assets) {}

  Pane(const Pane&) = delete;

  static inline void SetDrawColor(SDL_Renderer* renderer, const Color& c) {
    auto color = GetColor(c);

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  }

  static inline void FillRect(SDL_Renderer* renderer, int x, int y, int w, int h) {
    const SDL_Rect rc = { x, y, w, h };
    SDL_RenderFillRect(renderer, &rc);
  }

  static inline void RenderCopy(SDL_Renderer* renderer, SDL_Texture *texture, int x, int y, int w, int h) {
    const SDL_Rect rc = { x, y, w, h };
    SDL_RenderCopy(renderer, texture, nullptr, &rc);
  }

  static inline void RenderCopy(SDL_Renderer* renderer, SDL_Texture *texture, const SDL_Rect& rc) { SDL_RenderCopy(renderer, texture, nullptr, &rc); }

 protected:
  inline void RenderText(int x, int y, const Font& font, const std::string& text, utility::Color text_color) const {
    utility::RenderText(renderer_, x_ + x, y_ + y, assets_->GetFont(font), text, text_color);
  }

  inline void RenderCopy(utility::Texture& texture) { SDL_RenderCopy(renderer_, texture, nullptr, texture); }

  inline void SetDrawColor(const Color& c) const { SetDrawColor(renderer_, c); }

  inline void FillRect(int x, int y, int w, int h) const { FillRect(renderer_, x_ + x, y_ + y, w, h); }

  inline void RenderCopy(SDL_Texture* texture, int x, int y, int w, int h) const { RenderCopy(renderer_, texture, x_ + x, y_ + y, w, h); }

  inline void RenderCopy(SDL_Texture* texture, const SDL_Rect& rc) { SDL_RenderCopy(renderer_, texture, nullptr, &rc); }

  SDL_Renderer* renderer_;
  int x_;
  int y_;
  const std::shared_ptr<Assets>& assets_;
};

class TextPane : public Pane {
 public:
  using Texture = utility::Texture;

  enum class N { L1, L2 };
  enum class Orientation { Left, Right };

  static const int kBoxWidth = 148;
  static const int kBoxHeight = 84;
  static const int kBoxInteriorWidth = 138;
  static const int kBoxInteriorHeight = 74;

  TextPane(SDL_Renderer* renderer, int x, int y, const std::string& text, const std::shared_ptr<Assets>& assets)
      : Pane(renderer, x, y, assets), caption_texture_(Texture(renderer_, assets_->GetFont(Bold25), text, Color::White)) {}

  TextPane(SDL_Renderer* renderer, int x, int y, const std::shared_ptr<Assets>& assets) : Pane(renderer, x, y, assets) {}

  inline void SetCaptionOrientation(Orientation orientation) { orientation_ = orientation; }

  inline void SetCenteredText(int text, Color color = Color::SteelGray, const Font& font = Bold45) { SetCenteredText(std::to_string(text), color, font); }

  inline void SetCenteredText(size_t text, Color color = Color::SteelGray, const Font& font = Bold45) { SetCenteredText(std::to_string(text), color, font); }

  void SetCenteredText(const std::string& text, Color color = Color::SteelGray, const Font& font = Bold45) {
    lines_.resize(1);
    auto& line = lines_[0];
    line = Texture(renderer_, assets_->GetFont(font), text, color);
    line.SetX(line.center_x(kBoxWidth));
    line.SetY(line.center_y(kBoxHeight) + caption_texture_.height() + 5);
  }

  inline void ClearLines() { lines_.clear(); }

  inline void ClearBox() {
    SetDrawColor(Color::Black);
    FillRect(5, 10 + caption_texture_.height(), kBoxInteriorWidth, kBoxInteriorHeight);
  }

  void SetCenteredText(const std::string& text1, Color color1, const std::string& text2, Color color2) {
    lines_.resize(2);
    auto& line1 = lines_[0];
    line1 = Texture(renderer_, assets_->GetFont(Bold25), text1, color1);
    line1.SetX(line1.center_x(kBoxWidth));
    line1.SetY(caption_texture_.height() + 15);

    auto& line2 = lines_[1];
    line2 = Texture(renderer_, assets_->GetFont(Bold25), text2, color2);
    line2.SetX(line2.center_x(kBoxWidth));
    line2.SetY(line1.height() + 10 + (caption_texture_.height() + 5));
  }

  virtual void Render(double) override {
    RenderCaption();
    SetDrawColor(Color::Gray);
    FillRect(0, 5 + caption_texture_.height(), kBoxWidth, kBoxHeight);
    ClearBox();
    for (auto& line : lines_) {
      RenderCopy(line, line.x(), line.y(), line.width(), line.height());
    }
  }

 protected:
  inline void RenderCaption() {
    if (caption_texture_.is_null()) {
      return;
    }
    if (Orientation::Right == orientation_) {
      RenderCopy(caption_texture_, kBoxWidth - caption_texture_.width(), 0, caption_texture_.width(), caption_texture_.height());
    } else {
      RenderCopy(caption_texture_, 0, 0,  caption_texture_.width(), caption_texture_.height());
    }
  }

  std::vector<Texture> lines_;
  Texture caption_texture_;
  Orientation orientation_ = Orientation::Right;
};
