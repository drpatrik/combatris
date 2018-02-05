#include "game/tetromino_tspin_detection.h"

namespace {

const int kEmptyID =  static_cast<int>(Tetromino::Type::Empty);
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

Event::BonusMove DetectTSpin(const Matrix::Type& matrix, const Position& pos, int angle_index) {
  const auto& shape = kTSpin_Rotations.at(angle_index).shape_;
  int tspin_corners = 0;
  int tspin_minicorners = 0;

  for (size_t row = 0; row < shape.size(); ++row) {
    for (size_t col  = 0; col < shape.at(row).size(); ++col) {
      int try_row = pos.row() + row;
      int try_col = pos.col() + col;

      if (shape.at(row).at(col) == kTSpinCorner && matrix.at(try_row).at(try_col) != kEmptyID) {
        tspin_corners++;
      }
      if (shape.at(row).at(col) == kTSpinMiniCorner && matrix.at(try_row).at(try_col) != kEmptyID) {
        tspin_minicorners++;
      }
    }
  }
  Event::BonusMove bonus_move = Event::BonusMove::None;

  if (tspin_corners == 2 && tspin_minicorners >= 1) {
    bonus_move = Event::BonusMove::T_Spin;
  } else if (tspin_corners == 1 && tspin_minicorners >= 2) {
    bonus_move = Event::BonusMove::T_SpinMini;
  }

  return bonus_move;
}
