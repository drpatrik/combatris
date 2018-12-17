#pragma once

#include "game/events.h"
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
    if (ticks_ >= kDisplayTime) {
      texture_.reset();
    }
    if (!texture_.is_null()) {
      ClearBox();
      Pane::RenderCopy(texture_, texture_);
    }
  }

  virtual void Reset() override {
    got_lines_from_ = 0;
    EmptyQueue();
  }

  inline int lines() const { return lines_; }

  inline bool IsEmpty() const { return lines_ == 0; }

  inline size_t got_lines_from() const {
    assert(got_lines_from_);
    return got_lines_from_;
  }

  inline void EmptyQueue() {
    lines_ = 0;
    SetCenteredText(std::to_string(0));
  }

  inline bool got_lines() const { return lines_ > 0; }

  virtual void Update(const Event& event) override {
    if (Event::Type::SetCampaign == event.type()) {
      campaign_type_ = event.campaign_type();
    }
    if (!IsBattleCampaign(campaign_type_)) {
      return;
    }
    switch (event.type()) {
      case Event::Type::BattleNextTetrominoSuccessful:
        got_lines_from_ = 0;
        break;
      case Event::Type::BattleGotLines:
        texture_ = Texture(renderer_, assets_->GetFont(ObelixPro40), "+" + std::to_string(event.value1_), Color::Red);
        lines_ += event.value1_;
        got_lines_from_ = event.value2_;
        break;
      case Event::Type::CalculatedScore:
        texture_.reset();
        if (event.value2_as_int() == 0) {
          break;
        }
        if (lines_ - event.value2_as_int() < 0) {
          int lines_to_send = std::abs(lines_ - event.value2_as_int());

          events_.Push(Event::Type::BattleSendLines, lines_to_send);
          if (lines_ != 0) {
            texture_ = Texture(renderer_, assets_->GetFont(ObelixPro40), "-" + std::to_string(lines_to_send), Color::Green);
          }
          lines_ = 0;
        } else {
          lines_ -= event.value2_as_int();
          texture_ = Texture(renderer_, assets_->GetFont(ObelixPro40), "-" + std::to_string(event.value2_as_int()), Color::Green);
        }
        break;
      default:
        break;
    }
    if (IsIn(event, { Event::Type::CalculatedScore, Event::Type::BattleGotLines })) {
      ticks_ = 0.0;
      texture_.SetX(kX + utility::Center(kBoxWidth, texture_.width()));
      texture_.SetY(kY + utility::Center(kBoxHeight, texture_.height()));
      SetCenteredText(lines_);
    }
  }

 private:
  Events& events_;
  Texture texture_;
  int lines_ = 0;
  size_t got_lines_from_;
  double ticks_ = 0.0;
  CampaignType campaign_type_ = CampaignType::None;
};
