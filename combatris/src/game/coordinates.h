#pragma once

#include "game/constants.h"

#include <iostream>

inline int row_to_pixel(int row) { return static_cast<int>((row * kMinoHeight) + kMatrixStartY); }

inline int col_to_pixel(int col) { return static_cast<int>((col * kMinoWidth) + kMatrixStartX); }

inline int row_to_pixel_adjusted(int row) { return static_cast<int>((row * kMinoHeight) + (kMatrixStartY - (kMatrixFirstRow * kMinoHeight))); }

inline int col_to_pixel_adjusted(int col) { return static_cast<int>((col * kMinoWidth) + (kMatrixStartX - (kMatrixFirstCol * kMinoWidth))); }

inline int row_to_visible(int row) { return row - kMatrixFirstRow; }

inline int col_to_visible(int col) { return col - kMatrixFirstCol; }

class Position final {
 public:
  Position() = default;

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

  bool operator!=(const Position& rhs) const { return std::make_pair(row_, col_) != std::make_pair(rhs.row_, rhs.col_); }

  bool operator==(const std::pair<int, int>& rhs) const { return std::make_pair(row_, col_) == rhs; }

  void Print() const { std::cout << "Row: " << row_ << " Col:" << col_ << std::endl; }

 private:
  int row_ = -1;
  int col_ = -1;
};
