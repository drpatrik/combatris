#pragma once

struct Accumlator final {

  Accumlator() { Reset(); }

  void AddLines(int lines) {
    lines_ += lines;
    is_dirty_ = (lines > 0);
  }

  void AddLinesSent(int lines) {
    lines_sent_ += lines;
    is_dirty_ = (lines > 0);
  }

  void AddScore(int score) {
    score_ += score;
    is_dirty_ = (score > 0);
  }

  void AddKnockOut(int ko) {
    ko_ += ko;
    is_dirty_ = (ko > 0);
  }

  void SetLevel(int level) {
    level_ = level;
    is_dirty_ = true;
  }

  void Reset() {
    lines_ = 0;
    lines_sent_ = 0;
    score_ = 0;
    ko_ = 0;
    level_ = 0;
    is_dirty_ = false;
  }

  int lines_;
  int lines_sent_;
  int score_;
  int ko_;
  int level_;
  bool is_dirty_;
};
