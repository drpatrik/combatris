#pragma once

#include <vector>
#include <deque>
#include <iostream>

struct Line {
  Line(int row, const std::vector<int>& line) : row_(row), line_(line) {}

  int row_;
  std::vector<int> line_;
};

using Lines = std::vector<Line>;

enum class EventType { None, GameOver, NewGame, NextPiece, LinesCleared, LevelUp, CountDown, HardDrop, SoftDrop };

enum class SpecialMove { None, T_Spin, T_SpinMini, PerfectClear };

struct Event {
  Event(EventType type, const Lines& lines_cleared, SpecialMove special_move = SpecialMove::None)
      : type_(type), lines_cleared_(lines_cleared), special_move_(special_move) {}

  explicit Event(EventType type)
      : type_(type), lines_cleared_(), special_move_(SpecialMove::None) {}

  Event(EventType type, int lines_dropped)
      : type_(type), lines_cleared_(), special_move_(SpecialMove::None), lines_dropped_(lines_dropped) {}

  EventType type() const { return type_; }

  int lines_cleared() const { return lines_cleared_.size(); }

  EventType type_;
  Lines lines_cleared_;
  SpecialMove special_move_;
  int lines_dropped_ = 0;
};

class Events {
 public:
  Events() {}

  Events(const Events&) = delete;

  Events(const Events&&) = delete;

  bool IsEmpty() const { return events_.empty(); }

  void Push(const Event& event) { events_.push_back(event); }

  void Push(EventType type, const Lines& lines, SpecialMove special_move = SpecialMove::None) {
    events_.emplace_back(type, lines, special_move);
  }

  void Push(EventType type) { events_.emplace_back(type); }

  void Push(EventType type, int lines) { events_.emplace_back(type, lines); }

  Event Pop() {
    Event event = events_.front();

    events_.pop_front();
    return event;
  }

  void NewGame() { events_.clear(); }

 private:
  std::deque<Event> events_;
};
