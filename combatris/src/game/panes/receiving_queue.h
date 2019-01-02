#pragma once

#include "game/events.h"
#include "game/matrix.h"
#include "game/panes/pane.h"

#include <set>

class ReceivingQueue final : public TextPane, public EventListener {
 public:
  static const int kYOffs = 578;
  static const int kX = kMatrixStartX - kMinoWidth - (kBoxWidth + kSpace);
  static const int kY = kMatrixStartY + kYOffs + 5;
  static const int kInteriorWidth = (kBoxInteriorWidth >> 1) - 2;

  using Texture = utility::Texture;

  ReceivingQueue(SDL_Renderer* renderer, const std::shared_ptr<Assets>& assets, Events& events)
       : TextPane(renderer, kMatrixStartX - kMinoWidth - (kBoxWidth + kSpace),
                  (kMatrixStartY - kMinoHeight) + kYOffs, "LINES GOT", assets), events_(events) {
    lines_.resize(2);
    Reset();
  }

  virtual void Render(double delta_time) override {
    const auto kDisplayTime = 0.4;

    TextPane::RenderCaption();

    SetDrawColor(Color::Gray);
    FillRect(0, 5 + caption_texture_.height(), kBoxWidth, kBoxHeight);
    SetDrawColor(Color::Black);
    FillRect(5, 10 + caption_texture_.height(), kInteriorWidth, kBoxInteriorHeight);
    FillRect(kInteriorWidth + 9, 10 + caption_texture_.height(), kInteriorWidth, kBoxInteriorHeight);

    for (auto& line : lines_) {
      RenderCopy(line, line.x(), line.y(), line.width(), line.height());
    }
    ticks_ += delta_time;
    if (!texture_.is_null() && ticks_ >= kDisplayTime) {
      texture_.reset();
    }
    if (!texture_.is_null()) {
      ClearBox();
      Pane::RenderCopy(texture_, texture_);
    }
  }

  virtual void Reset() override {
    got_lines_from_.clear();
    total_lines_ = 0;
    ResetNewLines();
  }

  inline int new_lines() const { return new_lines_; }

  inline bool GotNewLines() const { return new_lines_ > 0; }

  void ResetNewLines() {
    new_lines_ = 0;
    Display();
  }

  void BroadcastKO() {
    for (const auto& host : got_lines_from_) {
      events_.Push(Event::Type::BattleKnockedOut, host);
    }
    Reset();
  }

  virtual void Update(const Event& event) override {
    if (Event::Type::SetCampaign == event.type()) {
      campaign_type_ = event.campaign_type();
    }
    if (!IsBattleCampaign(campaign_type_)) {
      return;
    }
    bool texture_update = false;

    switch (event.type()) {
      case Event::Type::BattleNextTetrominoSuccessful:
        got_lines_from_.clear();
        break;
      case Event::Type::BattleGotLines:
        texture_ = Texture(renderer_, assets_->GetFont(ObelixPro40), "+" + std::to_string(event.value1_), Color::Red);
        new_lines_ += event.value1_;
        total_lines_ += event.value1_;
        got_lines_from_.emplace(event.value2_);
        texture_update = true;
        break;
      case Event::Type::CalculatedScore:
        texture_update = UpdateQueue(event);
        break;
      default:
        break;
    }
    if (texture_update) {
      ticks_ = 0.0;
      texture_.SetX(kX + utility::Center(kBoxWidth, texture_.width()));
      texture_.SetY(kY + utility::Center(kBoxHeight, texture_.height()));
      Display();
    }
  }

 protected:
  bool UpdateQueue(const Event& event) {
    int lines_to_send = 0;
    int delta_lines = 0;

    if (new_lines_ > 0) {
      if (total_lines_ - event.value2_as_int() < 0) {
        lines_to_send = std::abs(total_lines_ - event.value2_as_int());
      }
      delta_lines = std::min(event.value2_as_int(), new_lines_);
      new_lines_ = std::max(new_lines_ - event.value2_as_int(), 0);
      total_lines_ = std::max(total_lines_- event.value2_as_int(), 0);
    } else if (total_lines_ > 0) {
      int lines = 0;

      for (const auto& line : event.lines_) {
        lines += static_cast<int>(Matrix::IsSolidLine(line));
      }
      if (lines == 0) {
        return false;
      }
      new_lines_ = std::max(new_lines_ - lines, 0);
      if (total_lines_ - lines <= 0) {
        lines_to_send = std::max(event.value2_as_int() - lines, 0);
      }
      total_lines_ = std::max(total_lines_- lines, 0);
      delta_lines = lines;
    } else {
      lines_to_send = event.value2_as_int();
    }
    if (lines_to_send > 0) {
      events_.Push(Event::Type::BattleSendLines, lines_to_send);
    }
    if (delta_lines > 0) {
      texture_ = Texture(renderer_, assets_->GetFont(ObelixPro40), "-" + std::to_string(delta_lines), Color::Green);
    }
    return (delta_lines > 0);
  }

  void Display() {
    auto& line1 = lines_[0];

    line1 = Texture(renderer_, assets_->GetFont(Bold45), std::to_string(new_lines_), Color::Red);
    line1.SetX(5 + line1.center_x(kInteriorWidth));
    line1.SetY(caption_texture_.height() + line1.center_y(kBoxHeight) + 5);

    auto& line2 = lines_[1];

    line2 = Texture(renderer_, assets_->GetFont(Bold45), std::to_string(total_lines_ - new_lines_), Color::Purple);
    line2.SetX(kInteriorWidth + 9 + line2.center_x(kInteriorWidth));
    line2.SetY(caption_texture_.height() + line2.center_y(kBoxHeight) + 5);
  }

 private:
  Events& events_;
  Texture texture_;
  int total_lines_ = 0;
  int new_lines_ = 0;
  double ticks_ = 0.0;
  std::set<size_t> got_lines_from_;
  CampaignType campaign_type_ = CampaignType::None;
};
