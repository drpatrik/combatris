#pragma once

#include "game/tetromino.h"
#include "game/events.h"

class Matrix final {
 public:
  using Type = std::vector<std::vector<int>>;

  Matrix(SDL_Renderer* renderer, Events& events, const std::vector<std::shared_ptr<const Tetromino>>& tetrominos)
      : renderer_(renderer), events_(events), tetrominos_(tetrominos) {
    Initialize();
  }

  // Used by test suit
  Matrix(const std::vector<std::vector<int>> &matrix, Events &events,
         const std::vector<std::shared_ptr<const Tetromino>> &tetrominos,
         SDL_Renderer *renderer = nullptr)
      : renderer_(renderer), events_(events), tetrominos_(tetrominos) {
    Initialize();

    for (int row = kVisibleRowStart; row < kVisibleRowEnd; ++row) {
      for (int col = kVisibleColStart; col < kVisibleColEnd; ++col) {
        master_matrix_.at(row).at(col) = matrix.at(row_to_visible(row)).at(col_to_visible(col));
      }
    }
    matrix_  = master_matrix_;
  }

  void Render();

  Events& GetEvents() { return events_; }

  bool IsValid(const Position& pos, const TetrominoRotationData& rotation_data) const;

  void Insert(const Position& pos, const TetrominoRotationData& rotation_data) {
    matrix_ = master_matrix_;
    Insert(matrix_, GetDropPosition(pos, rotation_data), rotation_data, true);
    Insert(matrix_, pos, rotation_data);
  }

  void Commit(Tetromino::Type type, Tetromino::Moves latest_move, const Position& pos, const TetrominoRotationData& rotation_data);

  Position GetDropPosition(const Position& current_pos, const TetrominoRotationData& rotation_data) const;

  void NewGame() { Initialize(); }

  void Print(bool master = true) const;

 protected:
  void Initialize();
  void Insert(Type& matrix, const Position& pos, const TetrominoRotationData& rotation_data, bool insert_ghost = false);

 private:
  friend bool operator==(const Matrix& rhs, const Matrix::Type& lhs);

  SDL_Renderer* renderer_;
  Events& events_;
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
