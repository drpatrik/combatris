#pragma once

#include "game/coordinates.h"

#include <deque>
#include <vector>

struct Line {
  Line(int row, const std::vector<int>& minos) : row_(row), minos_(minos) {}

  size_t size() const { return minos_.size(); }

  int row() const { return row_; }

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
    ScoringData,
    Score,
    Moves,
    LevelUp,
    PerfectClear,
    LinesCleared,
    OnFloor,
    Falling,
    SendLines,
    GotLines,
    CountdownAfterUnPauseDone
  };

  explicit Event(Type type)
      : type_(type), lines_cleared_() {}

  Event(Type type, const Lines& lines_cleared, const Position& pos, TSpinType tspin_type = TSpinType::None)
      : type_(type), lines_cleared_(lines_cleared), pos_(pos), tspin_type_(tspin_type) {}

  Event(Type type, const Lines& lines_cleared) : type_(type), lines_cleared_(lines_cleared) {}

  Event(Type type, const Position& pos, int score) : type_(type), pos_(pos), score_(score) {}

  Event(Type type, int lines_dropped)
      : type_(type), lines_cleared_(), tspin_type_(TSpinType::None), lines_dropped_(lines_dropped) {}

  Event(Type type, const Lines& lines_cleared, TSpinType tspin_type, ComboType combo_type, int combo_counter) :
      type_(type), lines_cleared_(lines_cleared), tspin_type_(tspin_type), combo_type_(combo_type), combo_counter_(combo_counter) {}

  inline bool Is(Event::Type type) const { return type == type_; }

  inline Type type() const { return type_; }

  inline bool IsDrop() const { return lines_dropped_ > 0; }

  inline int lines_cleared() const { return lines_cleared_.size(); }

  inline int score() const { return score_; }

  bool operator==(Event::Type type) { return type == type_; }

  Type type_;
  Lines lines_cleared_;
  Position pos_ = Position(-1, -1);
  TSpinType tspin_type_ = TSpinType::None;
  int lines_dropped_ = 0;
  int garbage_lines_ = 0;
  int score_ = 0;
  ComboType combo_type_ = ComboType::None;
  int combo_counter_ = 0;
};

class EventSink {
 public:
  virtual ~EventSink() noexcept {}

  virtual void Update(const Event& event) = 0;
};

class Events {
 public:
  Events() = default;

  Events(const Events&) = delete;

  inline bool IsEmpty() const { return events_.empty(); }

  void Push(Event::Type type, bool remove_if_already_exists = false) {
    if (remove_if_already_exists) {
      Remove(type);
    }
    events_.emplace_back(type);
  }

  inline void PushFront(Event::Type type) { events_.emplace_front(type); }

  inline void Push(const Event& event) { events_.push_back(event); }

  inline void Push(Event::Type type, const Lines& lines, const Position& pos, TSpinType tspin_type = TSpinType::None) {
    events_.emplace_back(type, lines, pos, tspin_type);
  }

  inline void Push(Event::Type type, const Lines& lines) { events_.emplace_back(type, lines); }

  inline void Push(Event::Type type, const Position& pos, int score) {
    events_.emplace_back(type, pos, score);
  }

  inline void Push(Event::Type type, int lines) { events_.emplace_back(type, lines); }

  inline void Push(Event::Type type, const Lines& lines_cleared, TSpinType tspin_type, ComboType combo_type, int counter) {
    events_.emplace_back(type, lines_cleared, tspin_type, combo_type, counter);
  }

  inline void Remove(Event::Type type) { events_.erase(std::remove(events_.begin(), events_.end(), type), events_.end()); }

  Event Pop() {
    auto event = events_.front();

    events_.pop_front();
    return event;
  }

  void Clear() { events_.clear(); }

 private:
  std::deque<Event> events_;
};
