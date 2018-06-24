#include "game/panes/moves.h"

namespace {

double kDisplayTime = 0.7;

const std::vector<std::string> kBasicScoreTypes = { "", "SINGLE", "DOUBLE", "TRIPLE", "TETRIS" };
const std::vector<std::string> kTSpinTypes = { "", "T-SPIN", "t-spin" };
const std::vector<std::string> kScoreModifiers = { "", "B2B #", "Combo #" };

std::string GetComboType(const Event& event) {
  std::string combo_type;

  if (ComboType::None != event.combo_type_) {
    if (ComboType::Combo == event.combo_type_) {
      combo_type = "Combo # " + std::to_string(event.combo_counter_);
    } else {
      combo_type = "B2B # " + std::to_string(event.combo_counter_);
    }
  }
  return combo_type;
}

} // namespace


void Moves::Update(const Event& event) {
  if (!event.Is(Event::Type::Moves)) {
    return;
  }
  auto line1 = GetComboType(event);
  auto line2 = kTSpinTypes[static_cast<int>(event.tspin_type_)];

  if (line2.empty()) {
    line2 = kBasicScoreTypes[event.lines()];
  }
  if (line1.empty()) {
    SetCenteredText(line2, Color::Red, Bold35);
  } else {
    SetCenteredText(line1, Color::Green, line2, Color::Red);
  }
  box_cleared_ = false;
  ticks_ = 0.0;
}

void Moves::Render(double delta_time) {
  TextPane::Render(delta_time);

  ticks_ += delta_time;
  if (!box_cleared_ && ticks_ >= kDisplayTime) {
    ClearBox();
    box_cleared_ = true;
  }
}
