#pragma once

#include "game/coordinates.h"

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
enum class ComboType { None, B2BTSpin, B2BTetris, Combo };

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
    LinesCleared,
    OnFloor,
    Falling,
    CountdownAfterUnPauseDone,
    GameOver,
    GameStatistics,
    PerfectClear,
    BattleStartGame,
    BattleResetCountDown,
    BattleSendLines,
    BattleGotLines,
    BattleKnockedOut,
    BattleYouDidKO,
    BattleWaitForPlayers,
    BattleNextTetrominoGotLines,
    BattleNextTetrominoSuccessful
  };

  enum class Value { None, DropScore, LinesSent, GotLines,  LevelUp,  };

  explicit Event(Type type) : type_(type), lines_cleared_() {}

  Event(Type type, const Lines& lines_cleared, const Position& pos, TSpinType tspin_type)
    : type_(type), lines_cleared_(lines_cleared), pos_(pos), tspin_type_(tspin_type) {}

  Event(Type type, const Lines& lines_cleared) : type_(type), lines_cleared_(lines_cleared) {}

  Event(Type type, const Position& pos, int score, int lines_sent, int lines_cleared) : type_(type), pos_(pos), score_(score), value_(lines_sent), value2_(lines_cleared) {}

  Event(Type type, int value) : type_(type), value_(value) {}

  Event(Type type, const Lines& lines_cleared, TSpinType tspin_type, ComboType combo_type, int combo_counter) :
      type_(type), lines_cleared_(lines_cleared), tspin_type_(tspin_type), combo_type_(combo_type), combo_counter_(combo_counter) {}

  operator Event::Type() const { return type_; }

  inline bool Is(Event::Type type) const { return type == type_; }

  inline Type type() const { return type_; }

  inline int lines_cleared() const { return static_cast<int>(lines_cleared_.size()); }

  inline bool operator==(Event::Type type) { return type == type_; }

  inline bool operator==(const Event& e) { return e.type_ == type_; }

  Type type_;
  Lines lines_cleared_;
  Position pos_ = Position(-1, -1);
  TSpinType tspin_type_ = TSpinType::None;
  ComboType combo_type_ = ComboType::None;
  int score_ = 0;
  int value_ = 0;
  int value2_ = 0;
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

  void Clear() { events_.clear(); }

  inline bool IsEmpty() const { return events_.empty(); }

 private:
  std::deque<Event> events_;
};
