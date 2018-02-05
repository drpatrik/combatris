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

struct Event {
  enum class Type { GameOver, NewGame, NextPiece, LinesCleared, LevelUp, CountDown, HardDrop, SoftDrop, PerfectClear, FloorReached };
  enum class BonusMove { None, T_Spin, T_SpinMini };

  Event(Type type, const Lines& lines_cleared, BonusMove bonus_move = BonusMove::None)
      : type_(type), lines_cleared_(lines_cleared), bonus_move_(bonus_move) {}

  explicit Event(Type type)
      : type_(type), lines_cleared_(), bonus_move_(BonusMove::None) {}

  Event(Type type, int lines_dropped)
      : type_(type), lines_cleared_(), bonus_move_(BonusMove::None), lines_dropped_(lines_dropped) {}

  Type type() const { return type_; }

  int lines_cleared() const { return lines_cleared_.size(); }

  Type type_;
  Lines lines_cleared_;
  BonusMove bonus_move_;
  int lines_dropped_ = 0;
};

class Events {
 public:
  Events() = default;

  Events(const Events&) = delete;

  Events(const Events&&) = delete;

  bool IsEmpty() const { return events_.empty(); }

  void Push(const Event& event) { events_.push_back(event); }

  void Push(Event::Type type, const Lines& lines, Event::BonusMove bonus_move = Event::BonusMove::None) {
    events_.emplace_back(type, lines, bonus_move);
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
