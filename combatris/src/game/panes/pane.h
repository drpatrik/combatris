#pragma once

#include "utility/color.h"
#include "game/assets.h"
#include "game/panes/pane_interface.h"

class Pane : public PaneInterface {
 public:
  Pane(SDL_Renderer* renderer, int x, int y, const std::shared_ptr<Assets>& assets) : renderer_(renderer), x_(x), y_(y), assets_(assets) {}

  Pane(const Pane&) = delete;

 protected:
  void RenderText(int x, int y, Font font, const std::string& text, Color text_color) const {
    ::RenderText(renderer_, x_ + x, y_ + y, assets_->GetFont(font), text, text_color);
  }

  void SetDrawColor(const Color& c) const {
    auto color = GetColor(c);

    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
  }

  void FillRect(int x, int y, int w, int h) const {
    SDL_Rect rc = { x_ + x, y_ + y, w, h };
    SDL_RenderFillRect(renderer_, &rc);
  }

  void RenderCopy(SDL_Texture *texture, int x, int y, int w, int h) const {
    SDL_Rect rc = { x_ + x, y_ + y, w, h };
    SDL_RenderCopy(renderer_, texture, nullptr, &rc);
  }

  SDL_Renderer* renderer_;
  int x_;
  int y_;
  const std::shared_ptr<Assets>& assets_;
};

class TextPane : public Pane {
 public:
  enum class N { L1, L2 };
  enum class Orientation { Left, Right };
  static const int kBoxWidth = 148;
  static const int kBoxHeight = 84;
  static const int kBoxInteriorWidth = 138;
  static const int kBoxInteriorHeight = 74;

  TextPane(SDL_Renderer* renderer, int x, int y, const std::string& text, const std::shared_ptr<Assets>& assets) : Pane(renderer, x, y, assets) {
    std::tie(caption_texture_, caption_width_, caption_height_) = CreateTextureFromText(renderer_, assets_->GetFont(Bold25), text, Color::White);
  }

  TextPane(SDL_Renderer* renderer, int x, int y, const std::shared_ptr<Assets>& assets) : Pane(renderer, x, y, assets) {}

  void SetCaptionOrientation(Orientation orientation) { orientation_ = orientation; }

  void SetCenteredText(int text) { SetCenteredText(std::to_string(text)); }

  void SetCenteredText(const std::string& text, Color color = Color::SteelGray, Font font = Bold45) {
    lines_.resize(1);
    auto& line = lines_[0];

    std::tie(line.texture_, line.w_, line.h_) = CreateTextureFromText(renderer_, assets_->GetFont(font), text, color);

    line.x_ = ((kBoxWidth - line.w_) / 2);
    line.y_ = ((kBoxHeight - line.h_) / 2) + (caption_height_ + 5);
  }

  void ClearBox() { lines_.clear(); }


  void SetCenteredText(const std::string& text1, Color color1, const std::string& text2, Color color2) {
    lines_.resize(2);
    auto& line1 = lines_[0];
    std::tie(line1.texture_, line1.w_, line1.h_) = CreateTextureFromText(renderer_, assets_->GetFont(Bold25), text1, color1);
    line1.x_ = ((kBoxWidth - line1.w_) / 2);
    line1.y_ = (caption_height_ + 15);

    auto& line2 = lines_[1];
    std::tie(line2.texture_, line2.w_, line2.h_) = CreateTextureFromText(renderer_, assets_->GetFont(Bold25), text2, color2);
    line2.x_ = ((kBoxWidth - line2.w_) / 2);
    line2.y_ = line1.h_ + 10 + (caption_height_ + 5);
  }

  virtual void Render(double) override {
    if (caption_texture_) {
      if (Orientation::Right == orientation_) {
        RenderCopy(caption_texture_.get(), kBoxWidth - caption_width_, 0, caption_width_, caption_height_);
      } else {
        RenderCopy(caption_texture_.get(), 0, 0, caption_width_, caption_height_);
      }
    }
    SetDrawColor(Color::Gray);
    FillRect(0, 5 + caption_height_, kBoxWidth, kBoxHeight);
    SetDrawColor(Color::Black);
    FillRect(5, 10 + caption_height_, kBoxInteriorWidth, kBoxInteriorHeight);
    for (const auto& line : lines_) {
      RenderCopy(line.texture_.get(), line.x_, line.y_, line.w_, line.h_);
    }
  }

 protected:
  struct TextLine {
    int x_ = 0;
    int y_ = 0;
    int w_ = 0;
    int h_ = 0;
    UniqueTexturePtr texture_ = nullptr;
  };
  std::vector<TextLine> lines_;
  int caption_width_ = 0;
  int caption_height_ = 0;
  UniqueTexturePtr caption_texture_ = nullptr;
  Orientation orientation_ = Orientation::Right;
};
