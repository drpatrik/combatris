#pragma once

struct Accumlator final {

  Accumlator() { Reset(); }

  void AddLines(int lines) { lines_ += lines; }

  void AddScore(int score) { score_ += score; }

  void SetLevel(int level) { level_ = level; }

  void AddLinesSent(int lines) { lines_sent_ += lines; }

  void AddKnockedOut(int ko) { ko_ += ko; }

  void Reset() {
    lines_ = 0;
    score_ = 0;
    level_ = 0;
    lines_sent_ = 0;
    ko_ = 0;
  }

  int lines_;
  int score_;
  int level_;
  int lines_sent_;
  int ko_;
};
