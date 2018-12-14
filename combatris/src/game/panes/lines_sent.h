#pragma once

#include "game/events.h"
#include "game/panes/pane.h"

class LinesSent final : public TextPane, public EventListener {
 public:
  // 578
  LinesSent(SDL_Renderer* renderer, int offset, const std::shared_ptr<Assets>& assets)
       : TextPane(renderer, kMatrixStartX - kMinoWidth - (kBoxWidth + kSpace),
                  (kMatrixStartY - kMinoHeight) + offset, "LINES SENT", assets) { Reset(); }

  virtual void Reset() override { lines_sent_ = 0;  SetCenteredText(std::to_string(0)); }

  virtual void Update(const Event& event) override {
    if (!event.Is(Event::Type::BattleSendLines)) {
      return;
    }
    lines_sent_ += event.value1_;
    SetCenteredText(lines_sent_);
  }

 private:
  int lines_sent_ = 0;
};
