#pragma once

#include "game/asset_manager.h"
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
        int adjusted_row = row - kVisibleRowStart;
        int adjusted_col = col - kVisibleColStart;

        ingame_matrix_.at(row).at(col) =
            matrix.at(adjusted_row).at(adjusted_col);
      }
    }
    master_matrix_ = ingame_matrix_;
  }

  void Render();

  Events& GetEventQueue() { return events_; }

  bool IsValid(const Position& pos, const TetrominoRotationData& rotation_data) const;

  void Insert(const Position& pos, const TetrominoRotationData& rotation_data) {
    ingame_matrix_ = master_matrix_;
    Insert(ingame_matrix_, GetDropPosition(pos, rotation_data), rotation_data, true);
    Insert(ingame_matrix_, pos, rotation_data);
  }

  void Commit(const Position& pos, const TetrominoRotationData& rotation_data);

  Position GetDropPosition(const Position& current_pos, const TetrominoRotationData& rotation_data) const;

  void NewGame() { Initialize(); }

  void Print() const;

 protected:
  void Initialize();
  void Insert(Type& matrix, const Position& pos, const TetrominoRotationData& rotation_data, bool insert_ghost = false);

 private:
  friend bool operator==(const Matrix& rhs, const Matrix::Type& lhs);

  SDL_Renderer* renderer_;
  Events& events_;
  std::vector<std::shared_ptr<const Tetromino>> tetrominos_;
  Type ingame_matrix_;
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
