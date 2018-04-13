#include "game/panes/scoring.h"

namespace {

const std::vector<int> kScoreForLines = { 0, 100, 300, 500, 800 };
const std::vector<int> kLinesToSendForLines =  { 0, 0, 1, 2, 4 };
const std::vector<int> kScoreForTSpin = { 400, 800, 1200, 1600 };
const std::vector<int> kLinesToSendTSpin =  { 0, 0, 2, 4, 6 };
const std::vector<int> kB2BScoreForTSpin = { 0, 1200, 1800, 2700 };
const std::vector<int> kB2BSLinesToSendForTSprin = { 0, 2, 3, 6, 9 };
const std::vector<int> kLinesToSendForCombo = { 0, 1, 2, 4, 6, 9, 12, 16, 20, 24, 28, 32 };

} // namespace

void Scoring::UpdateEvents(int score, ComboType combo_type, int lines_to_send, const Event& event) {
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
  if (event.lines_cleared() > 0) {
    events_.Push(Event::Type::LinesCleared, event.lines_cleared_);
  }
  events_.Push(Event::Type::CalculatedScore, event.pos_, score);
  events_.Push(Event::Type::Moves, event.lines_cleared_, event.tspin_type_, combo_type, counter);
  if (lines_to_send > 0) {
    Event event(Event::Type::BattleSendLines);

    event.lines_ = lines_to_send;
    events_.Push(event);
  }
}

std::tuple<int, int, ComboType, int> Scoring::Calculate(const Event& event) {
  auto lines_to_send = 0;
  auto base_score = 0;
  auto combo_score = 0;
  auto combo_type = ComboType::None;

  if (event.IsDrop()) {
    return std::make_tuple(base_score, combo_score, combo_type, lines_to_send);
  }
  ++combo_counter_;
  switch (event.tspin_type_) {
    case TSpinType::None:
      lines_to_send = kLinesToSendForLines.at(event.lines_cleared());
      base_score = kScoreForLines.at(event.lines_cleared());
      if (event.lines_cleared() == 4) {
        if (++b2b_counter_ > 1) {
          combo_score = 1200;
          lines_to_send += 6;
          combo_type = ComboType::B2BTetris;
        }
      } else if (event.lines_cleared() > 0) {
        b2b_counter_ = 0;
        if (combo_counter_ > 1) {
          lines_to_send += kLinesToSendForCombo.at(combo_counter_);
          combo_score = (combo_counter_ - 1) * ((combo_counter_ < 3) ? 50 : 100);
          combo_type = ComboType::Combo;
        }
      } else {
        combo_counter_ = 0;
      }
      break;
    case TSpinType::TSpin:
    case TSpinType::TSpinMini:
      if (TSpinType::TSpinMini == event.tspin_type_) {
        if (event.lines_cleared() > 0) {
          lines_to_send += 1;
          base_score = 200;
        } else {
          b2b_counter_ = 0;
          base_score =  100;
        }
      } else {
        lines_to_send = kScoreForTSpin.at(event.lines_cleared());
        base_score = kScoreForTSpin.at(event.lines_cleared());
      }
      if (event.lines_cleared() > 0 && ++b2b_counter_ > 1) {
        combo_score = kB2BScoreForTSpin.at(event.lines_cleared());
        combo_type = ComboType::B2BTSpin;
        lines_to_send += kB2BSLinesToSendForTSprin.at(event.lines_cleared());
      }
      break;
  }
  return std::make_tuple(base_score, combo_score, combo_type, lines_to_send);
}

void Scoring::Update(const Event& event) {
  if (!event.Is(Event::Type::ScoringData) && !event.Is(Event::Type::PerfectClear)) {
    return;
  }
  if (event.Is(Event::Type::PerfectClear)) {
    events_.Push(Event::Type::BattleSendLines, 10);
    return;
  }
  auto [base_score, combo_score, combo_type, lines_to_send] = Calculate(event);

  auto score = (base_score * level_->level()) + (combo_score * level_->level());

  UpdateEvents(score, combo_type, lines_to_send, event);

  score_ += event.lines_dropped_;
  score_ += score;

  TextPane::SetCenteredText(score_, Color::SteelGray, Normal35);
}
