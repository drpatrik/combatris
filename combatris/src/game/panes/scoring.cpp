#include "game/panes/scoring.h"

namespace {

const std::vector<int> kScoreForLines = { 0, 100, 300, 500, 800 };
const std::vector<int> kScoreForTSpin = { 400, 800, 1200, 1600 };
const std::vector<int> kB2BScoreForTSpin = { 0, 1200, 1800, 2700 };

} // namespace

void Scoring::UpdateEvents(int score, ComboType combo_type, const Event& event) {
  if (score == 0) {
    return;
  }
  int counter = 0;

  switch (combo_type) {
    case ComboType::None:
      break;
    case ComboType::B2BTSpin:
    case ComboType::B2BTetris:
      counter = b2b_counter_ - 1;
      break;
    case ComboType::Combo:
      counter = combo_counter_ - 1;
      break;
  }
  events_.Push(Event::Type::LinesCleared, event.lines_cleared_);
  events_.Push(Event::Type::CalculatedScore, event.pos_, score);
  events_.Push(Event::Type::Moves, event.lines_cleared_, event.tspin_type_, combo_type, counter);
}

std::tuple<int, int, ComboType> Scoring::Calculate(const Event& event) {
  auto base_score = 0;
  auto combo_score = 0;
  auto combo_type = ComboType::None;

  if (event.IsDrop()) {
    return std::make_tuple(base_score, combo_score, combo_type);
  }
  ++combo_counter_;
  switch (event.tspin_type_) {
    case TSpinType::None:
    case TSpinType::TSpinMini:
      if (TSpinType::TSpinMini == event.tspin_type_) {
        base_score = (event.lines_cleared() == 0) ? 100 : 200;
      } else {
        base_score = kScoreForLines.at(event.lines_cleared());
      }
      if (event.lines_cleared() == 4 && ++b2b_counter_ > 1) {
        combo_score = 1200;
        combo_type = ComboType::B2BTetris;
      } else if (event.lines_cleared() > 0 && combo_counter_ > 1) {
        b2b_counter_ = 0;
        combo_score = ((combo_counter_ - 1) * 50);
        combo_type = ComboType::Combo;
      } else if (event.lines_cleared() == 0) {
        ClearCounters();
      }
      break;
    case TSpinType::TSpin:
      base_score = kScoreForTSpin.at(event.lines_cleared());
      if (event.lines_cleared() > 0 && ++b2b_counter_ > 1) {
        combo_score = kB2BScoreForTSpin.at(event.lines_cleared());
        combo_type = ComboType::B2BTSpin;
      }
      break;
  }
  return std::make_tuple(base_score, combo_score, combo_type);
}

void Scoring::Update(const Event& event) {
  if (!event.Is(Event::Type::ScoringData)) {
    return;
  }
  auto [base_score, combo_score, combo_type] = Calculate(event);

  auto score = (base_score * level_->level()) + (combo_score * level_->level());

  UpdateEvents(score, combo_type, event);

  score_ += event.lines_dropped_;
  score_ += score;

  TextPane::SetCenteredText(score_, Color::SteelGray, Normal35);
}
