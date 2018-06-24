#include "game/panes/scoring.h"

namespace {

const std::vector<int> kScoreForLines = { 0, 100, 300, 500, 800 };
const std::vector<int> kLinesToSendForLines =  { 0, 0, 1, 2, 4 };
const std::vector<int> kScoreForTSpin = { 400, 800, 1200, 1600 };
const std::vector<int> kLinesToSendTSpin =  { 0, 0, 2, 4, 6 };
const std::vector<int> kB2BScoreForTSpin = { 0, 1200, 1800, 2700 };
const std::vector<int> kB2BSLinesToSendForTSprin = { 0, 2, 3, 6, 9 };
const std::vector<int> kLinesToSendForCombo = { 0, 1, 2, 4, 6, 9, 12, 16, 20, 24, 28, 32 };
const std::vector<int> kLinesToClear = { 0, 1, 3, 5, 8 };
const std::vector<int> kLinesToClearTSpin = { 4, 8, 12, 16 };

} // namespace

void Scoring::Update(const Event& event) {
  if (!IsIn(event, { Event::Type::ClearedLinesScoreData, Event::Type::DropScoreData, Event::Type::PerfectClear, Event::Type::SetCampaign, Event::Type::SetStartLevel, Event::Type::LevelUp })) {
    return;
  }
  switch (event) {
    case Event::Type::SetCampaign:
      rule_type_ = CampaignToRuleType(ToCampaignType(event.value_));
      break;
    case Event::Type::SetStartLevel:
      start_level_ = event.value_;
      break;
    case Event::Type::LevelUp:
      level_ = event.value_;
      break;
    case Event::Type::ClearedLinesScoreData: {
      auto [base_score, combo_score, combo_type, lines_to_send, lines_to_clear] = Calculate(event);

      auto score = (base_score * level_) + (combo_score * level_);

      if (event.Is(Event::Type::PerfectClear)) {
        lines_to_send += 10;
      }
      UpdateEvents(score, combo_type, lines_to_send, lines_to_clear, event);
      score_ += score;
      DisplayScore(score_);
      break;
    }
    case Event::Type::DropScoreData:
      score_ += event.value_;
      DisplayScore(score_);
      break;
    default:
      break;
  }
}

void Scoring::DisplayScore(int score) {
  std::tie(score_texture_, rc_.w, rc_.h) = CreateTextureFromText(renderer_, assets_->GetFont(ObelixPro40), std::to_string(score), Color::Yellow);
  rc_.x = x_ - rc_.w;
  rc_.y = y_ - rc_.h;
}

void Scoring::UpdateEvents(int score, ComboType combo_type, int lines_to_send, int lines_to_clear, const Event& event) {
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
  lines_to_clear = (CampaignRuleType::Normal == rule_type_) ? event.lines() : lines_to_clear;

  if (lines_to_clear > 0) {
    events_.Push(Event::Type::LinesCleared, event.lines_, lines_to_clear);
  }
  events_.Push(Event::Type::CalculatedScore, event.pos_, score, lines_to_send);
  events_.Push(Event::Type::Moves, event.lines_, event.tspin_type_, combo_type, counter);
}

std::tuple<int, int, ComboType, int, int> Scoring::Calculate(const Event& event) {
  auto lines_to_send = 0;
  auto lines_to_clear = 0;
  auto base_score = 0;
  auto combo_score = 0;
  auto combo_type = ComboType::None;

  ++combo_counter_;
  switch (event.tspin_type_) {
    case TSpinType::None:
      lines_to_send = kLinesToSendForLines.at(event.lines());
      lines_to_clear = kLinesToClear.at(event.lines());
      base_score = kScoreForLines.at(event.lines());
      if (event.lines() == 4) {
        if (++b2b_counter_ > 1) {
          combo_score = 1200;
          lines_to_send += 2;
          combo_type = ComboType::B2BTetris;
        }
      } else if (event.lines() > 0) {
        b2b_counter_ = 0;
        if (combo_counter_ > 1) {
          lines_to_send += kLinesToSendForCombo.at(combo_counter_ - 1);
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
        if (event.lines() > 0) {
          lines_to_send += 1;
          lines_to_clear += 2;
          base_score = 200;
        } else {
          b2b_counter_ = 0;
          lines_to_clear += 1;
          base_score =  100;
        }
      } else {
        lines_to_send = kScoreForTSpin.at(event.lines());
        lines_to_clear = kLinesToClearTSpin.at(event.lines());
        base_score = kScoreForTSpin.at(event.lines());
      }
      if (event.lines() > 0 && ++b2b_counter_ > 1) {
        combo_score = kB2BScoreForTSpin.at(event.lines());
        combo_type = ComboType::B2BTSpin;
        lines_to_send += kB2BSLinesToSendForTSprin.at(event.lines());
      }
      break;
  }
  if (combo_counter_ > 1 || b2b_counter_ > 1) {
    lines_to_clear += static_cast<int>(0.5 * lines_to_clear);
  }
  return std::make_tuple(base_score, combo_score, combo_type, lines_to_send, lines_to_clear);
}
