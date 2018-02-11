#pragma once

#include "game/constants.h"

#include <iostream>

inline int row_to_pixel(size_t row) { return static_cast<int>((row * kBlockHeight) + kMatrixStartY); }

inline int col_to_pixel(size_t col) { return static_cast<int>((col * kBlockWidth) + kMatrixStartX); }

inline int row_to_pixel_adjusted(size_t row) { return static_cast<int>((row * kBlockHeight) + (kMatrixStartY - (kVisibleRowStart * kBlockHeight))); }

inline int col_to_pixel_adjusted(size_t col) { return static_cast<int>((col * kBlockWidth) + (kMatrixStartX - (kVisibleColStart  * kBlockWidth))); }

inline int row_to_visible(int row) { return row - kVisibleRowStart; }

inline int col_to_visible(int col) { return col - kVisibleColStart; }

class Position final {
 public:
  inline Position(int row, int col) : row_(row), col_(col) {}

  inline int row() const { return row_; }

  inline int col() const { return col_; }

  inline int inc_row() { row_++; return row_; }

  inline int dec_row() { row_--; return row_; }

  inline int inc_col() { col_++; return col_; }

  inline int dec_col() { col_--; return col_; }

  inline int x() const { return col_to_pixel(col_); }

  inline int y() const { return row_to_pixel(row_); }

  bool operator<(const Position& rhs) const { return std::make_pair(row_, col_) < std::make_pair(rhs.row_, rhs.col_); }

  bool operator==(const Position& rhs) const { return std::make_pair(row_, col_) == std::make_pair(rhs.row_, rhs.col_); }

  bool operator==(const std::pair<int, int>& rhs) const { return std::make_pair(row_, col_) == rhs; }

  void Print() const { std::cout << "Row: " << row_ << " Col:" << col_ << std::endl; }

 private:
  int row_ = -1;
  int col_ = -1;
};

inline int Center(int w1, int w2 ) {
  return std::abs(w1 - w2) / 2;
}
