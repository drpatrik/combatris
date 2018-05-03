#include "game/tetromino_tspin_detection.h"

namespace {

const int kEmptyID =  static_cast<int>(Tetromino::Type::Empty);
const int kBombID = static_cast<int>(Tetromino::Type::Bomb);
const int kTSpinCorner = 1;
const int kTSpinMiniCorner = 2;

const TetrominoRotationData kTSpinShape_0D({
    {1, 6, 1},
    {6, 6, 6},
    {2, 0, 2}
  });

const TetrominoRotationData kTSpinShape_90D({
    {2, 6, 1},
    {0, 6, 6},
    {2, 6, 1}
  });

const TetrominoRotationData kTSpinShape_180D({
    {2, 0, 2},
    {6, 6, 6},
    {1, 6, 1}
  });

const TetrominoRotationData kTSpinShape_270D({
    {1, 6, 2},
    {6, 6, 0},
    {1, 6, 2}
  });

const std::vector<TetrominoRotationData> kTSpin_Rotations = {
  kTSpinShape_0D,
  kTSpinShape_90D,
  kTSpinShape_180D,
  kTSpinShape_270D
};

} // namespace

TSpinType DetectTSpin(const Matrix::Type& matrix, const Position& pos, int angle_index) {
  const auto& shape = kTSpin_Rotations.at(angle_index).shape_;
  auto tspin_corners = 0;
  auto tspin_minicorners = 0;

  for (size_t row = 0; row < shape.size(); ++row) {
    for (size_t col  = 0; col < shape.at(row).size(); ++col) {
      const auto elem = matrix.at(pos.row() + row).at(pos.col() + col);

      if (shape.at(row).at(col) == kTSpinCorner && elem != kEmptyID && elem != kBombID) {
        tspin_corners++;
      }
      if (shape.at(row).at(col) == kTSpinMiniCorner && elem != kEmptyID && elem != kBombID) {
        tspin_minicorners++;
      }
    }
  }
  auto tspin_type = TSpinType::None;

  if (tspin_corners == 2 && tspin_minicorners >= 1) {
    tspin_type = TSpinType::TSpin;
  } else if (tspin_corners == 1 && tspin_minicorners >= 2) {
    tspin_type = TSpinType::TSpinMini;
  }

  return tspin_type;
}
