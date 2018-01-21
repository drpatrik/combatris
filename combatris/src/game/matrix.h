#pragma once

#include "asset_manager.h"

struct Line {
  Line(int row_in_matrix, const std::vector<int>& line) : row_in_matrix_(row_in_matrix), line_(line) {}

  int row_in_matrix_;
  std::vector<int> line_;
};

class Matrix final {
 public:
  using Type = std::vector<std::vector<int>>;
  using Lines = std::vector<Line>;

  Matrix(SDL_Renderer* renderer, const std::vector<std::shared_ptr<const Tetromino>>& tetrominos)
      : renderer_(renderer), tetrominos_(tetrominos) {
    Initialize();
  }

  void Render();

  bool IsValid(const Position& pos, const TetrominoRotationData& rotation_data) const;

  void Insert(const Position& pos, const TetrominoRotationData& rotation_data) {
    ingame_matrix_ = master_matrix_;
    Insert(ingame_matrix_, GetDropPosition(pos, rotation_data), rotation_data, true);
    Insert(ingame_matrix_, pos, rotation_data);
  }

  Lines Commit(const Position& pos, const TetrominoRotationData& rotation_data);

  Position GetDropPosition(const Position& current_pos, const TetrominoRotationData& rotation_data) const;

  void NewGame() { Initialize(); }

  void Print();

 protected:
  void Initialize();
  void Insert(Type& matrix, const Position& pos, const TetrominoRotationData& rotation_data, bool insert_ghost = false);

 private:
  SDL_Renderer* renderer_;
  std::vector<std::shared_ptr<const Tetromino>> tetrominos_;
  Type ingame_matrix_;
  Type master_matrix_;
};
