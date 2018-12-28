#pragma once

#include "game/events.h"
#include "game/matrix.h"
#include "game/panes/pane.h"

class ReceivingQueue final : public TextPane, public EventListener {
 public:
  static const int kYOffs = 578;
  static const int kX = kMatrixStartX - kMinoWidth - (kBoxWidth + kSpace);
  static const int kY = kMatrixStartY + kYOffs + 5;

  using Texture = utility::Texture;

  ReceivingQueue(SDL_Renderer* renderer, const std::shared_ptr<Assets>& assets, Events& events)
       : TextPane(renderer, kMatrixStartX - kMinoWidth - (kBoxWidth + kSpace),
                  (kMatrixStartY - kMinoHeight) + kYOffs, "LINES GOT", assets), events_(events) { Reset(); }

  virtual void Render(double delta_time) override {
    const auto kDisplayTime = 0.4;

    TextPane::Render(delta_time);

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
    got_lines_from_ = 0;
    total_lines_ = 0;
    EmptyQueue();
  }

  inline int new_lines() const { return new_lines_; }

  inline size_t got_lines_from() const {
    assert(got_lines_from_);
    return got_lines_from_;
  }

  inline void EmptyQueue() {
    new_lines_ = 0;
    Display();
  }

  inline bool GotNewLines() const { return new_lines_ > 0; }

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
        got_lines_from_ = 0;
        break;
      case Event::Type::BattleGotLines:
        texture_ = Texture(renderer_, assets_->GetFont(ObelixPro40), "+" + std::to_string(event.value1_), Color::Red);
        new_lines_ += event.value1_;
        total_lines_ += event.value1_;
        got_lines_from_ = event.value2_;
        texture_update = true;
        break;
      case Event::Type::CalculatedScore:
        texture_update = CalculateLinesToSend(event);
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
  bool CalculateLinesToSend(const Event& event) {
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

  void Display() { SetCenteredText(std::to_string(new_lines_) + "(" + std::to_string(total_lines_) + ")"); }

 private:
  Events& events_;
  Texture texture_;
  int total_lines_ = 0;
  int new_lines_ = 0;
  size_t got_lines_from_;
  double ticks_ = 0.0;
  CampaignType campaign_type_ = CampaignType::None;
};
