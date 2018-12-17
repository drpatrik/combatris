#pragma once

#include "game/events.h"
#include "game/panes/pane.h"

class Moves final : public TextPane, public EventListener {
 public:
  Moves(SDL_Renderer* renderer, const std::shared_ptr<Assets>& assets) :
      TextPane(renderer,  kMatrixEndX + kMinoWidth + kSpace, (kMatrixStartY - kMinoHeight) + 578 + 32, assets) {
    SetCaptionOrientation(TextPane::Orientation::Left);
  }

  virtual void Reset() override {
    ClearLines();
    box_cleared_ = true;
  }

  virtual void Update(const Event& event) override;

  virtual void Render(double) override;

 private:
  double ticks_ = 0;
  bool box_cleared_ = false;
};
