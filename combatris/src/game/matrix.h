#pragma once

#include "game/events.h"
#include "game/tetromino.h"
#include "game/panes/pane_interface.h"

class Matrix final : public PaneInterface {
 public:
  using Type = std::vector<std::vector<int>>;
  using CommitReturnTyoe = std::tuple<Lines, TSpinType, bool>;

  Matrix(SDL_Renderer* renderer, const std::vector<std::shared_ptr<const Tetromino>>& tetrominos)
      : renderer_(renderer), tetrominos_(tetrominos) {
    Initialize();
  }

  // Used by test suit
  Matrix(const std::vector<std::vector<int>> &matrix,
         const std::vector<std::shared_ptr<const Tetromino>> &tetrominos)
      : tetrominos_(tetrominos) {
    Initialize();
    SetTestData(matrix);
  }

  void SetTestData(const std::vector<std::vector<int>> &matrix) {
    Initialize();

    for (int row = kVisibleRowStart; row < kVisibleRowEnd; ++row) {
      for (int col = kVisibleColStart; col < kVisibleColEnd; ++col) {
        master_matrix_.at(row).at(col) = matrix.at(row_to_visible(row)).at(col_to_visible(col));
      }
    }
    matrix_  = master_matrix_;
  }

  virtual void Render(double) override;

  virtual void Reset() override { Initialize(); }

  bool IsValid(const Position& pos, const TetrominoRotationData& rotation_data) const;

  void Insert(const Position& pos, const TetrominoRotationData& rotation_data) {
    matrix_ = master_matrix_;
    Insert(matrix_, GetDropPosition(pos, rotation_data), rotation_data, true);
    Insert(matrix_, pos, rotation_data);
  }

  CommitReturnTyoe Commit(Tetromino::Type type, Tetromino::Move latest_move, const Position& pos, const TetrominoRotationData& rotation_data);

  Position GetDropPosition(const Position& current_pos, const TetrominoRotationData& rotation_data) const;

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
};

inline bool operator==(const Matrix& rhs, const Matrix::Type& lhs) {
  for (int row = kVisibleRowStart; row < kVisibleRowEnd; ++row) {
    const auto& line = rhs.master_matrix_.at(row);
    if (!std::equal(line.begin() + kVisibleColStart, line.end() - kVisibleColStart, lhs.at(row - kVisibleRowStart).begin())) {
      return false;
    }
  }

  return true;
}
