#pragma once

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
    GameOver,
    NewGame,
    NextPiece,
    Scoring,
    LevelUp,
    CountDown,
    PerfectClear,
    FloorReached,
    SendGarbage,
    GotGarbage
  };

  Event(Type type, const Lines& lines_cleared, TSpinType tspin_type = TSpinType::None)
      : type_(type), lines_cleared_(lines_cleared), tspin_type_(tspin_type) {}

  explicit Event(Type type)
      : type_(type), lines_cleared_(), tspin_type_(TSpinType::None) {}

  Event(Type type, int lines_dropped)
      : type_(type), lines_cleared_(), tspin_type_(TSpinType::None), lines_dropped_(lines_dropped) {}

  Type type() const { return type_; }

  int lines_cleared() const { return lines_cleared_.size(); }

  Type type_;
  Lines lines_cleared_;
  TSpinType tspin_type_;
  int lines_dropped_ = 0;
  int garbage_lines_ = 0;
};

class Events {
 public:
  Events() = default;

  Events(const Events&) = delete;

  Events(const Events&&) = delete;

  bool IsEmpty() const { return events_.empty(); }

  void Push(const Event& event) { events_.push_back(event); }

  void Push(Event::Type type, const Lines& lines, TSpinType tspin_type = TSpinType::None) {
    events_.emplace_back(type, lines, tspin_type);
  }

  void Push(Event::Type type) { events_.emplace_back(type); }

  void Push(Event::Type type, int lines) { events_.emplace_back(type, lines); }

  Event Pop() {
    Event event = events_.front();

    events_.pop_front();
    return event;
  }

  void NewGame() { events_.clear(); }

 private:
  std::deque<Event> events_;
};
