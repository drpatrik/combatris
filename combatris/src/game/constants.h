#pragma once

const int kVisibleRows = 20;
const int kVisibleCols = 10;
const int kVisibleRowStart = 2;
const int kVisibleColStart = 2;
const int kVisibleRowEnd = kVisibleRows + kVisibleRowStart;
const int kVisibleColEnd = kVisibleCols + kVisibleColStart;
const int kRows = kVisibleRows + 3;
const int kCols = kVisibleCols + 4;
const int kBlockWidth = 32;
const int kBlockHeight = 32;
const int kMatrixHeight = (kVisibleRows * kBlockHeight);
const int kMatrixWidth = (kVisibleCols * kBlockWidth);
const int kWidth = (kVisibleCols * kBlockWidth) + 400 + (kBlockWidth * 2);
const int kHeight = (kVisibleRows * kBlockHeight) + 40 + (kBlockHeight * 2);
const int kNormalFontSize = 25;
const int kSmallFontSize = 15;
const int kLargeFontSize = 45;
const int kMatrixStartX = 200 + kBlockWidth;
const int kMatrixEndX = kMatrixStartX + (kVisibleCols * kBlockWidth);
const int kMatrixStartY = 20 + kBlockHeight;
const int kMatrixEndY = kMatrixStartY + (kVisibleRows * kBlockHeight);
const int kFrameStartX = kMatrixStartX - kBlockWidth;
const int kFrameStartY = kMatrixStartY - kBlockHeight;
const int kFrameRows = kVisibleRows + 2;
const int kFrameCols = kVisibleCols + 2;
