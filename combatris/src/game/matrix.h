#pragma once

#include "game/tetromino_generator.h"

class Matrix final {
 public:
  enum class ValidationAction {JustValidate, InsertIfValid};

  using Type = std::vector<std::vector<int>>;

  Matrix(SDL_Renderer* renderer, const TetrominoGenerator& tetromino_generator) : renderer_(renderer), tetrominos_(tetromino_generator.GetTetrominos()) {
    Initialize();
  }

  void Print();

  void Render();

  bool IsValid(const Position& pos, const TetrominoRotationData& rotation_data, ValidationAction validation_action = ValidationAction::JustValidate);

  void Insert(const Position& pos, const TetrominoRotationData& rotation_data);

 protected:
  void Initialize();

 private:
  SDL_Renderer* renderer_;
  std::vector<std::shared_ptr<const Tetromino>> tetrominos_;
  std::shared_ptr<TetrominoGenerator> tetromino_generator_;
  Type ingame_matrix_;
  Type master_matrix_;
};
