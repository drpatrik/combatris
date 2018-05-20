#pragma once

struct Accumlator final {

  Accumlator() { Reset(); }

  void AddLines(int lines) {
    lines_ += lines;
    is_dirty_ = (lines > 0);
  }

  void AddScore(int score) {
    score_ += score;
    is_dirty_ = (score > 0);
  }

  void SetLevel(int level) {
    level_ = level;
    is_dirty_ = true;
  }

  void Reset() {
    lines_ = 0;
    score_ = 0;
    level_ = 0;
    is_dirty_ = false;
  }

  bool IsDirty() {
    if (is_dirty_) {
      is_dirty_ = false;
      return true;
    }
    return false;
  }

  int lines_;
  int score_;
  int level_;

 private:
  bool is_dirty_;
};
