#pragma once

#include <cstddef>

const int kFPS = 60;
const double kFrameDelay(double(1000000.0) / double(kFPS));
const int kRows = 20;
const int kCols = 10;
const int kBlockWidth = 54;
const int kBlockHeight = 54;
const int kWidth = kCols * kBlockWidth;
const int kHeight = kRows * kBlockHeight;
const int kNormalFontSize = 25;
const int kSmallFontSize = 15;
const int kLargeFontSize = 45;
const size_t kNumTetrominos = 7;
const int kBoardStartX = 0;
const int kBoardEndX = kBoardStartX + (kCols * kBlockWidth);
const int kBoardStartY = 0;
const int kBoardEndY = kBoardStartY + (kRows * kBlockHeight);
