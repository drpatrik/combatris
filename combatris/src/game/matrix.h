#pragma once

#include "game/events.h"
#include "game/tetromino.h"
#include "game/panes/pane_interface.h"

#include <tuple>

class Matrix final : public PaneInterface {
 public:
  using Type = std::vector<std::vector<int>>;
  using CommitReturnType = std::tuple<Lines, TSpinType, bool>;

  Matrix(SDL_Renderer* renderer, const std::vector<std::shared_ptr<const Tetromino>>& tetrominos)
      : renderer_(renderer), tetrominos_(tetrominos) { Initialize(); }

  // Used by test suit
  Matrix(const std::vector<std::vector<int>> &matrix,
         const std::vector<std::shared_ptr<const Tetromino>> &tetrominos)
      : tetrominos_(tetrominos) {
    Initialize();
    SetTestData(matrix);
  }

  Matrix(const Matrix&) = delete;

  void SetTestData(const std::vector<std::vector<int>> &matrix) {
    Initialize();

    for (int row = kMatrixFirstRow; row < kMatrixLastRow; ++row) {
      for (int col = kMatrixFirstCol; col < kMatrixLastCol; ++col) {
        master_matrix_.at(row).at(col) = matrix.at(row_to_visible(row)).at(col_to_visible(col));
      }
    }
    matrix_  = master_matrix_;
  }

  const Type& data() const { return matrix_; }

  Type& data() { return matrix_; }

  bool IsDirty() {
    bool ret_value = false;

    std::swap(ret_value, is_dirty_);

    return ret_value;
  }

  virtual void Render(double) override;

  virtual void Reset() override { Initialize(); }

  bool HasSolidLines() const;

  bool InsertSolidLines(int lines);

  void RemoveSolidLines();

  bool IsAboveSkyline(const Position& pos, const TetrominoRotationData& rotation_data) const;

  bool IsValid(const Position& pos, const TetrominoRotationData& rotation_data) const;

  void Insert(const Position& pos, const TetrominoRotationData& rotation_data) {
    is_dirty_ = true;
    matrix_ = master_matrix_;
    Insert(matrix_, GetDropPosition(pos, rotation_data), rotation_data, true);
    Insert(matrix_, pos, rotation_data);
  }

  Position GetDropPosition(const Position& current_pos, const TetrominoRotationData& rotation_data) const;

  auto Commit(Tetromino::Type type, Tetromino::Angle angle, Tetromino::Move latest_move, const Position& current_pos) {
    return Commit(type, latest_move, current_pos, tetrominos_.at(static_cast<int>(type) - 1)->GetRotationData(angle));
  }

  CommitReturnType Commit(Tetromino::Type type, Tetromino::Move latest_move, const Position& pos, const TetrominoRotationData& rotation_data);

  void Print(bool master = true) const;

 protected:
  void Initialize();

  void Insert(Type& matrix, const Position& pos, const TetrominoRotationData& rotation_data, bool insert_ghost = false);

 private:
  friend bool operator==(const Matrix& rhs, const Matrix::Type& lhs);

  SDL_Renderer* renderer_ = nullptr;
  std::vector<std::shared_ptr<const Tetromino>> tetrominos_;
  Type matrix_;
  Type master_matrix_;
  bool is_dirty_ = false;
};

inline bool operator==(const Matrix& rhs, const Matrix::Type& lhs) {
  for (int row = kMatrixFirstRow; row < kMatrixLastRow; ++row) {
    const auto& line = rhs.master_matrix_.at(row);
    if (!std::equal(line.begin() + kMatrixFirstCol, line.end() - kMatrixFirstCol, lhs.at(row - kMatrixFirstRow).begin())) {
      return false;
    }
  }

  return true;
}
