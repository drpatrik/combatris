#pragma once

#include "game/coordinates.h"

#include <deque>
#include <vector>

struct Line {
  Line(int row, const std::vector<int>& line) : row_(row), line_(line) {}

  int row_;
  std::vector<int> line_;
};

using Lines = std::vector<Line>;

enum class TSpinType { None, TSpin, TSpinMini };

struct Event {
  enum class Type {
    None,
    Pause,
    UnPause,
    GameOver,
    NewGame,
    NextPiece,
    Scoring,
    ScoreAnimation,
    LevelUp,
    ResetGame,
    PerfectClear,
    FloorReached,
    InTransit,
    SendLines,
    GotLines,
    AnimationDone
  };

  Event(Type type, const Lines& lines_cleared, const Position& pos, TSpinType tspin_type = TSpinType::None)
      : type_(type), lines_cleared_(lines_cleared), pos_(pos), tspin_type_(tspin_type) {}

  explicit Event(Type type)
      : type_(type), lines_cleared_() {}

  Event(Type type, const Position& pos, int score) : type_(type), pos_(pos), score_(score) {}

  Event(Type type, int lines_dropped)
      : type_(type), lines_cleared_(), tspin_type_(TSpinType::None), lines_dropped_(lines_dropped) {}

  inline bool Is(Event::Type type) const { return type == type_; }

  inline Type type() const { return type_; }

  inline bool IsDrop() const { return lines_dropped_ > 0; }

  inline int lines_cleared() const { return lines_cleared_.size(); }

  inline int score() const { return score_; }

  Type type_;
  Lines lines_cleared_;
  Position pos_ = Position(-1, -1);
  TSpinType tspin_type_ = TSpinType::None;
  int lines_dropped_ = 0;
  int garbage_lines_ = 0;
  int score_ = 0;
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

  Events(const Events&&) = delete;

  bool IsEmpty() const { return events_.empty(); }

  void Push(Event::Type type) { events_.emplace_back(type); }

  void PushFront(Event::Type type) { events_.emplace_front(type); }

  void Push(const Event& event) { events_.push_back(event); }

  void Push(Event::Type type, const Lines& lines, const Position& pos, TSpinType tspin_type = TSpinType::None) {
    events_.emplace_back(type, lines, pos, tspin_type);
  }

  void Push(Event::Type type, const Position& pos, int score) {
    events_.emplace_back(type, pos, score);
  }

  void Push(Event::Type type, int lines) { events_.emplace_back(type, lines); }

  Event Pop() {
    Event event = events_.front();

    events_.pop_front();
    return event;
  }

  void Clear() { events_.clear(); }

 private:
  std::deque<Event> events_;
};
