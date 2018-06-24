#pragma once

struct Accumlator final {

  Accumlator() { Reset(1); }

  void AddLines(int lines) { lines_ += lines; }

  void AddScore(int score) { score_ += score; }

  void SetLevel(int level) { level_ = level; }

  void Reset(int start_level) {
    lines_ = 0;
    score_ = 0;
    level_ = start_level;
  }

  int lines_;
  int score_;
  int level_;
};
