#pragma once

#include "tools/color.h"

#include <vector>

struct TetrominoMetadata {
  TetrominoMetadata(const std::vector<std::vector<int>> &shape,int renderer_hint, Color color) : shape_(shape), renderer_hint_(renderer_hint), color_(GetColor(color)) {}
  TetrominoMetadata(const std::vector<std::vector<int>> &shape) : shape_(shape), renderer_hint_(0), color_(GetColor(Color::Black)) {}
  std::vector<std::vector<int>> shape_;
  int width() { return shape_.at(0).size(); }
  int height() { return shape_.size(); }
  int renderer_hint_;
  SDL_Color color_;
};

// I_Block 1

const TetrominoMetadata kTetrominoMetadataI_BlockA0({
    {1, 1, 1, 1}
  }, 0, Color::Cyan);

const TetrominoMetadata kTetrominoMetadataI_BlockA90(
    {{1},
    {1},
    {1},
    {1}});

const std::vector<TetrominoMetadata> kTetrominoI_Blocks = {
  kTetrominoMetadataI_BlockA0,
  kTetrominoMetadataI_BlockA90,
  kTetrominoMetadataI_BlockA0,
  kTetrominoMetadataI_BlockA90
};

// J_Block 2

const TetrominoMetadata kTetrominoMetadataJ_BlockA0({
    {2, 2, 2},
    {0, 0, 2}
  }, 1, Color::Blue);

const TetrominoMetadata kTetrominoMetadataJ_BlockA90({
    {0, 2},
    {0, 2},
    {2, 2}
  });

const TetrominoMetadata kTetrominoMetadataJ_BlockA180({
    {2, 0, 0},
    {2, 2, 2}
  });

const TetrominoMetadata kTetrominoMetadataJ_BlockA270({
    {2, 2},
    {2, 0},
    {2, 0}
  });

const std::vector<TetrominoMetadata> kTetrominoJ_Blocks = {
  kTetrominoMetadataJ_BlockA0,
  kTetrominoMetadataJ_BlockA90,
  kTetrominoMetadataJ_BlockA180,
  kTetrominoMetadataJ_BlockA270
};

// L_Block 3

const TetrominoMetadata kTetrominoMetadataL_BlockA0({
    {3, 3, 3},
    {3, 0, 0}
  }, 1, Color::Orange);

const TetrominoMetadata kTetrominoMetadataL_BlockA90({
    {3, 3},
    {0, 3},
    {0, 3}
  });

const TetrominoMetadata kTetrominoMetadataL_BlockA180({
    {0, 0, 3},
    {3, 3, 3}
  });

const TetrominoMetadata kTetrominoMetadataL_BlockA270({
    {3, 0},
    {3, 0},
    {3, 3}
  });

const std::vector<TetrominoMetadata> kTetrominoL_Blocks = {
  kTetrominoMetadataL_BlockA0,
  kTetrominoMetadataL_BlockA90,
  kTetrominoMetadataL_BlockA180,
  kTetrominoMetadataL_BlockA270
};

// O_Block 4

const TetrominoMetadata kTetrominoMetadataO_BlockA0({
    {4, 4},
    {4, 4}
  }, 0, Color::Yellow);

const std::vector<TetrominoMetadata> kTetrominoO_Blocks = {
  kTetrominoMetadataO_BlockA0,
  kTetrominoMetadataO_BlockA0,
  kTetrominoMetadataO_BlockA0,
  kTetrominoMetadataO_BlockA0
};

// S_Block 5

const TetrominoMetadata kTetrominoMetadataS_BlockA0({
    {0, 5, 5},
    {5, 5, 0}
  }, 1, Color::Green);

const TetrominoMetadata kTetrominoMetadataS_BlockA90({
    {5, 0},
    {5, 5},
    {0, 5}
  });

const std::vector<TetrominoMetadata> kTetrominoS_Blocks = {
  kTetrominoMetadataS_BlockA0,
  kTetrominoMetadataS_BlockA90,
  kTetrominoMetadataS_BlockA0,
  kTetrominoMetadataS_BlockA90
};


// T_Block 6

const TetrominoMetadata kTetrominoMetadataT_BlockA0({
    {6, 6, 6},
    {0, 6, 0}
  }, 1, Color::Purple);

const TetrominoMetadata kTetrominoMetadataT_BlockA90({
    {0, 6},
    {6, 6},
    {0, 6}
  });

const TetrominoMetadata kTetrominoMetadataT_BlockA180({
    {0, 6, 0},
    {6, 6, 6}
  });

const TetrominoMetadata kTetrominoMetadataT_BlockA270({
    {6, 0},
    {6, 6},
    {6, 0}
  });

const std::vector<TetrominoMetadata> kTetrominoT_Blocks = {
  kTetrominoMetadataT_BlockA0,
  kTetrominoMetadataT_BlockA90,
  kTetrominoMetadataT_BlockA180,
  kTetrominoMetadataT_BlockA270
};

// Z_Block 7

const TetrominoMetadata kTetrominoMetadataZ_BlockA0({
    {7, 7, 0},
    {0, 7, 7}
  }, 0, Color::Red);

const TetrominoMetadata kTetrominoMetadataZ_BlockA90({
    {0, 7},
    {7, 7},
    {7, 0}
  });

const std::vector<TetrominoMetadata> kTetrominoZ_Blocks = {
  kTetrominoMetadataZ_BlockA0,
  kTetrominoMetadataZ_BlockA90,
  kTetrominoMetadataZ_BlockA0,
  kTetrominoMetadataZ_BlockA90
};
