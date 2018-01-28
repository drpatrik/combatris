#pragma once

#include <vector>
#include <deque>
#include <iostream>

struct Line {
  Line(int row_in_matrix, const std::vector<int>& line) : row_in_matrix_(row_in_matrix), line_(line) {}

  int row_in_matrix_;
  std::vector<int> line_;
};

using Lines = std::vector<Line>;

enum class EventType { None, NextPiece, LinesCleared, LevelUp, GameOver, NewGame };

enum class SpecialMove { None, T_Spin, T_SpinMini };

struct Event {
  Event(EventType type, const Lines& lines_cleared, SpecialMove special_move = SpecialMove::None)
      : type_(type), lines_cleared_(lines_cleared), special_move_(special_move) {}

  explicit Event(EventType type)
      : type_(type), lines_cleared_(), special_move_(SpecialMove::None) {}

  EventType type() const { return type_; }

  int lines_cleared() const { return lines_cleared_.size(); }

  EventType type_;
  Lines lines_cleared_;
  SpecialMove special_move_;
};

class Events {
 public:
  Events() {}

  Events(const Events&) = delete;

  Events(const Events&&) = delete;

  void Push(const Event& event) { events_.push_back(event); }

  void Push(EventType type, const Lines& lines, SpecialMove special_move = SpecialMove::None) {
    events_.emplace_back(type, lines, special_move);
  }

  void Push(EventType type) { events_.emplace_back(type); }

  Event Pop() {
    if (events_.empty()) {
      return Event(EventType::None);
    }
    Event event = events_.front();

    events_.pop_front();
    return event;
  }

  void NewGame() { events_.clear(); }

 private:
  std::deque<Event> events_;
};
