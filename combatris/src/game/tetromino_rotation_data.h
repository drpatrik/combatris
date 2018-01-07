#pragma once

#include <vector>

struct TetrominoRotationData {
  explicit TetrominoRotationData(const std::vector<std::vector<int>> &data) : data_(data) {}
  std::vector<std::vector<int>> data_;
};

// I_Block 1

const TetrominoRotationData kTetrominoRotationData_I_0D({
    {0, 0, 0, 0},
    {1, 1, 1, 1},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  });

const TetrominoRotationData kTetrominoRotationData_I_90D({
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0}
  });

const TetrominoRotationData kTetrominoRotationData_I_180D({
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {1, 1, 1, 1},
    {0, 0, 0, 0}
  });

const TetrominoRotationData kTetrominoRotationData_I_270D({
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0}
  });

const std::vector<TetrominoRotationData> kTetromino_I_Rotations = {
  kTetrominoRotationData_I_0D,
  kTetrominoRotationData_I_90D,
  kTetrominoRotationData_I_180D,
  kTetrominoRotationData_I_270D
};

// J_Block 2

const TetrominoRotationData kTetrominoRotationData_J_0D({
    {2, 0, 0},
    {2, 2, 2},
    {0, 0, 0}
  });

const TetrominoRotationData kTetrominoRotationData_J_90D({
    {0, 2, 2},
    {0, 2, 0},
    {0, 2, 0}
  });

const TetrominoRotationData kTetrominoRotationData_J_180D({
    {0, 0, 0},
    {2, 2, 2},
    {0, 0, 2}
  });

const TetrominoRotationData kTetrominoRotationData_J_270D({
    {0, 2, 0},
    {0, 2, 0},
    {2, 2, 0}
  });

const std::vector<TetrominoRotationData> kTetromino_J_Rotations = {
  kTetrominoRotationData_J_0D,
  kTetrominoRotationData_J_90D,
  kTetrominoRotationData_J_180D,
  kTetrominoRotationData_J_270D
};

// L_Block 3

const TetrominoRotationData kTetrominoRotationData_L_0D({
    {0, 0, 3},
    {3, 3, 3},
    {0, 0, 0}
  });

const TetrominoRotationData kTetrominoRotationData_L_90D({
    {0, 3, 0},
    {0, 3, 0},
    {0, 3, 3}
  });

const TetrominoRotationData kTetrominoRotationData_L_180D({
    {0, 0, 0},
    {3, 3, 3},
    {3, 0, 0}
  });

const TetrominoRotationData kTetrominoRotationData_L_270D({
    {3, 3, 0},
    {0, 3, 0},
    {0, 3, 0}
  });

const std::vector<TetrominoRotationData> kTetromino_L_Rotations = {
  kTetrominoRotationData_L_0D,
  kTetrominoRotationData_L_90D,
  kTetrominoRotationData_L_180D,
  kTetrominoRotationData_L_270D
};

// O_Block 4

const TetrominoRotationData kTetrominoRotationData_O_0D({
    {0, 4, 4, 0},
    {0, 4, 4, 0},
    {0, 0, 0, 0}
  });

const std::vector<TetrominoRotationData> kTetromino_O_Rotations = {
  kTetrominoRotationData_O_0D,
  kTetrominoRotationData_O_0D,
  kTetrominoRotationData_O_0D,
  kTetrominoRotationData_O_0D
};

// S_Block 5

const TetrominoRotationData kTetrominoRotationData_S_0D({
    {0, 5, 5},
    {5, 5, 0},
    {0, 0, 0}
  });

const TetrominoRotationData kTetrominoRotationData_S_90D({
    {0, 5, 0},
    {0, 5, 5},
    {0, 0, 5}
  });

const TetrominoRotationData kTetrominoRotationData_S_180D({
    {0, 0, 0},
    {0, 5, 5},
    {5, 5, 0}
  });

const TetrominoRotationData kTetrominoRotationData_S_270D({
    {5, 0, 0},
    {5, 5, 0},
    {0, 5, 0}
  });

const std::vector<TetrominoRotationData> kTetromino_S_Rotations = {
  kTetrominoRotationData_S_0D,
  kTetrominoRotationData_S_90D,
  kTetrominoRotationData_S_180D,
  kTetrominoRotationData_S_270D
};

// T_Block 6

const TetrominoRotationData kTetrominoRotationData_T_0D({
    {0, 6, 0},
    {6, 6, 6},
    {0, 0, 0}
  });

const TetrominoRotationData kTetrominoRotationData_T_90D({
    {0, 6, 0},
    {0, 6, 6},
    {0, 6, 0}
  });

const TetrominoRotationData kTetrominoRotationData_T_180D({
    {0, 0, 0},
    {6, 6, 6},
    {0, 6, 0}
  });

const TetrominoRotationData kTetrominoRotationData_T_270D({
    {0, 6, 0},
    {6, 6, 0},
    {0, 6, 0}
  });

const std::vector<TetrominoRotationData> kTetromino_T_Rotations = {
  kTetrominoRotationData_T_0D,
  kTetrominoRotationData_T_90D,
  kTetrominoRotationData_T_180D,
  kTetrominoRotationData_T_270D
};

// Z_Block 7

const TetrominoRotationData kTetrominoRotationData_Z_0D({
    {7, 7, 0},
    {0, 7, 7},
    {0, 0, 0}
  });

const TetrominoRotationData kTetrominoRotationData_Z_90D({
    {0, 0, 7},
    {0, 7, 7},
    {0, 7, 0}
  });

const TetrominoRotationData kTetrominoRotationData_Z_180D({
    {0, 0, 0},
    {7, 7, 0},
    {0, 7, 7}
  });

const TetrominoRotationData kTetrominoRotationData_Z_270D({
    {0, 7, 0},
    {7, 7, 0},
    {7, 0, 0}
  });

const std::vector<TetrominoRotationData> kTetromino_Z_Rotations = {
  kTetrominoRotationData_Z_0D,
  kTetrominoRotationData_Z_90D,
  kTetrominoRotationData_Z_180D,
  kTetrominoRotationData_Z_270D
};
