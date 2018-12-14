#pragma once

#include "game/coordinates.h"
#include "game/combatris_types.h"

#include <deque>
#include <vector>
#include <algorithm>

struct Line {
  Line(int row, const std::vector<int>& minos) : row_(row), minos_(minos) {}

  int row_;
  std::vector<int> minos_;
};

using Lines = std::vector<Line>;

enum class TSpinType { None, TSpin, TSpinMini };
enum class ComboType { None, B2BTSpin, B2BCombatris, Combo };

struct Event {
  enum class Type {
    None,
    Pause,
    UnPause,
    NewGame,
    NextTetromino,
    DropScoreData,
    ClearedLinesScoreData,
    CalculatedScore,
    Moves,
    LevelUp,
    SetStartLevel,
    LinesCleared,
    OnFloor,
    ClearOnFloor,
    CountdownAfterUnPauseDone,
    GameOver,
    GameStatistics,
    PerfectClear,
    SetCampaign,
    NewTime,
    SprintClearedAll,
    MenuSetModeAndCampaign,
    MultiplayerCampaignOver,
    MultiplayerRejected,
    MultiplayerStartGame,
    MultiplayerResetCountDown,
    ShowSplashScreen,
    BattleSendLines,
    BattleGotLines,
    BattleKnockedOut,
    BattleYouDidKO,
    BattleNextTetrominoGotLines,
    BattleNextTetrominoSuccessful
  };

  inline explicit Event(Type type) : type_(type), lines_() {}

  inline Event(Type type, const Lines& lines_cleared, const Position& pos, TSpinType tspin_type, int has_solid_lines)
      : type_(type), lines_(lines_cleared), pos_(pos), tspin_type_(tspin_type), value1_(has_solid_lines) {}

  inline Event(Type type, const Lines& lines, int lines_to_clear) : type_(type), lines_(lines), value1_(lines_to_clear) {}

  inline Event(Type type, const Position& pos, int score, int lines_sent) : type_(type), pos_(pos), value1_(score), value2_(lines_sent) {}

  inline Event(Type type, int value) : type_(type), value1_(value) {}

  inline Event(Type type, size_t value) : type_(type), value2_(value) {}

  inline Event(Type type, ModeType mode, CampaignType campaign) : type_(type), value1_(ToInt(mode)), value2_(ToInt(campaign)) {}

  inline Event(Type type, CampaignType campaign_type) : type_(type), value2_(ToInt(campaign_type)) {}

  inline Event(Type type, const Lines& lines_cleared, TSpinType tspin_type, ComboType combo_type, int combo_counter) :
      type_(type), lines_(lines_cleared), tspin_type_(tspin_type), combo_type_(combo_type), combo_counter_(combo_counter) {}

  inline operator Event::Type() const { return type_; }

  inline bool Is(Event::Type type) const { return type == type_; }

  inline Type type() const { return type_; }

  inline bool operator==(Event::Type type) { return type == type_; }

  inline bool operator==(const Event& e) { return e.type_ == type_; }

  inline int lines() const { return static_cast<int>(lines_.size()); }

  inline ModeType mode_type() const { return static_cast<ModeType>(value1_); }

  inline CampaignType campaign_type() const { return static_cast<CampaignType>(value2_); }

  inline bool has_solid_lines() const { return value1_ == 1; }

  inline int score() const { return value1_; }

  inline int value2_as_int() const { return static_cast<int>(value2_); }

  Type type_;
  Lines lines_;
  Position pos_ = Position(-1, -1);
  TSpinType tspin_type_ = TSpinType::None;
  ComboType combo_type_ = ComboType::None;
  int value1_ = 0;
  size_t value2_ = 0;
  int combo_counter_ = 0;
};

inline bool IsIn(Event::Type type, const std::initializer_list<Event::Type>& list) {
  return std::find(list.begin(), list.end(), type) != list.end();
}

class EventListener {
 public:
  virtual ~EventListener() noexcept {}

  virtual void Update(const Event& event) = 0;
};

class Events {
 public:
  enum class QueueRule { AllowDuplicates, NoDuplicates };

  Events() = default;

  Events(const Events&) = delete;

  Events(const Events&&) noexcept = delete;

  void Push(Event::Type type, QueueRule queue_rule = QueueRule::AllowDuplicates) {
    if (Event::Type::None == type) {
      return;
    }
    if (QueueRule::NoDuplicates == queue_rule) {
      Remove(type);
    }
    events_.emplace_back(type);
  }

  template<class ...Args>
  void Push(Args&&... args) { events_.emplace_back(std::forward<Args>(args)...); }

  inline void PushFront(Event::Type type) { events_.emplace_front(type); }

  inline void Remove(Event::Type type) { events_.erase(std::remove(events_.begin(), events_.end(), type), events_.end()); }

  Event Pop() {
    auto event = events_.front();

    events_.pop_front();
    return event;
  }

  inline void Clear() { events_.clear(); }

  inline bool IsEmpty() const { return events_.empty(); }

 private:
  std::deque<Event> events_;
};
