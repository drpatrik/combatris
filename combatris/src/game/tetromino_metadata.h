#pragma once

#include "tools/color.h"

#include <vector>

struct TetrominoMetadata {
  TetrominoMetadata(const std::vector<std::vector<int>> &shape, Color color) : shape_(shape), color_(GetColor(color)) {}
  std::vector<std::vector<int>> shape_;
  int width() { return shape_.at(0).size(); }
  int height() { return shape_.size(); }
  SDL_Color color_;
};

// I_Block 1

const TetrominoMetadata kTetrominoMetadata_I_0D({
    {1, 1, 1, 1}
  }, Color::Cyan);

const TetrominoMetadata kTetrominoMetadata_I_90D(
    {{1},
    {1},
    {1},
    {1}}, Color::Cyan);

const std::vector<TetrominoMetadata> kTetromino_I_Rotations = {
  kTetrominoMetadata_I_0D,
  kTetrominoMetadata_I_90D,
  kTetrominoMetadata_I_0D,
  kTetrominoMetadata_I_90D
};

// J_Block 2

const TetrominoMetadata kTetrominoMetadata_J_0D({
    {2, 0, 0},
    {2, 2, 2}
  }, Color::Blue);

const TetrominoMetadata kTetrominoMetadata_J_90D({
    {2, 2},
    {2, 0},
    {2, 0}
  }, Color::Blue);

const TetrominoMetadata kTetrominoMetadata_J_180D({
    {2, 2, 2},
    {0, 0, 2}
  }, Color::Blue);

const TetrominoMetadata kTetrominoMetadata_J_270D({
    {0, 2},
    {0, 2},
    {2, 2}
  }, Color::Blue);

const std::vector<TetrominoMetadata> kTetromino_J_Rotations = {
  kTetrominoMetadata_J_0D,
  kTetrominoMetadata_J_90D,
  kTetrominoMetadata_J_180D,
  kTetrominoMetadata_J_270D
};

// L_Block 3

const TetrominoMetadata kTetrominoMetadata_L_0D({
    {0, 0, 3},
    {3, 3, 3}
  }, Color::Orange);

const TetrominoMetadata kTetrominoMetadata_L_90D({
    {3, 0},
    {3, 0},
    {3, 3}
  }, Color::Orange);

const TetrominoMetadata kTetrominoMetadata_L_180D({
    {3, 3, 3},
    {3, 0, 0}
  }, Color::Orange);

const TetrominoMetadata kTetrominoMetadata_L_270D({
    {3, 3},
    {0, 3},
    {0, 3}
  }, Color::Orange);

const std::vector<TetrominoMetadata> kTetromino_L_Rotations = {
  kTetrominoMetadata_L_0D,
  kTetrominoMetadata_L_90D,
  kTetrominoMetadata_L_180D,
  kTetrominoMetadata_L_270D
};

// O_Block 4

const TetrominoMetadata kTetrominoMetadata_O_0D({
    {4, 4},
    {4, 4}
  }, Color::Yellow);

const std::vector<TetrominoMetadata> kTetromino_O_Rotations = {
  kTetrominoMetadata_O_0D,
  kTetrominoMetadata_O_0D,
  kTetrominoMetadata_O_0D,
  kTetrominoMetadata_O_0D
};

// S_Block 5

const TetrominoMetadata kTetrominoMetadata_S_0D({
    {0, 5, 5},
    {5, 5, 0}
  }, Color::Green);

const TetrominoMetadata kTetrominoMetadata_S_90D({
    {5, 0},
    {5, 5},
    {0, 5}
  }, Color::Green);

const std::vector<TetrominoMetadata> kTetromino_S_Rotations = {
  kTetrominoMetadata_S_0D,
  kTetrominoMetadata_S_90D,
  kTetrominoMetadata_S_0D,
  kTetrominoMetadata_S_90D
};


// T_Block 6

const TetrominoMetadata kTetrominoMetadata_T_0D({
    {0, 6, 0},
    {6, 6, 6}
  }, Color::Purple);

const TetrominoMetadata kTetrominoMetadata_T_90D({
    {6, 0},
    {6, 6},
    {6, 0}
  }, Color::Purple);

const TetrominoMetadata kTetrominoMetadata_T_180D({
    {6, 6, 6},
    {0, 6, 0}
  }, Color::Purple);

const TetrominoMetadata kTetrominoMetadata_T_270D({
    {0, 6},
    {6, 6},
    {0, 6}
  }, Color::Purple);

const std::vector<TetrominoMetadata> kTetromino_T_Rotations = {
  kTetrominoMetadata_T_0D,
  kTetrominoMetadata_T_90D,
  kTetrominoMetadata_T_180D,
  kTetrominoMetadata_T_270D
};

// Z_Block 7

const TetrominoMetadata kTetrominoMetadata_Z_0D({
    {7, 7, 0},
    {0, 7, 7}
  }, Color::Red);

const TetrominoMetadata kTetrominoMetadata_Z_90D({
    {0, 7},
    {7, 7},
    {7, 0}
  }, Color::Red);

const std::vector<TetrominoMetadata> kTetromino_Z_Rotations = {
  kTetrominoMetadata_Z_0D,
  kTetrominoMetadata_Z_90D,
  kTetrominoMetadata_Z_0D,
  kTetrominoMetadata_Z_90D
};
