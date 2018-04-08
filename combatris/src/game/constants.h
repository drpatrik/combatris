#pragma once

const int kVisibleRows = 20;
const int kVisibleCols = 10;
const int kVisibleRowStart = 2;
const int kVisibleColStart = 2;
const int kVisibleRowEnd = kVisibleRows + kVisibleRowStart;
const int kVisibleColEnd = kVisibleCols + kVisibleColStart;
const int kRows = kVisibleRows + 3;
const int kCols = kVisibleCols + 4;
const int kMinoWidth = 32;
const int kMinoHeight = 32;
const int kMatrixHeight = (kVisibleRows * kMinoHeight);
const int kMatrixWidth = (kVisibleCols * kMinoWidth);
const int kMultiPlayerPaneWidth = 220;
const int kMultiPlayerPaneHeight = kMatrixHeight + (kMinoHeight * 2);
const int kWidth = ((kVisibleCols * kMinoWidth) + 400 + (kMinoWidth * 2)) + kMultiPlayerPaneWidth + 24;
const int kHeight = (kVisibleRows * kMinoHeight) + 40 + (kMinoHeight * 2);
const int kMatrixStartX = 200 + kMinoWidth;
const int kMatrixEndX = kMatrixStartX + (kVisibleCols * kMinoWidth);
const int kMatrixStartY = 20 + kMinoHeight;
const int kMatrixEndY = kMatrixStartY + (kVisibleRows * kMinoHeight);
const int kFrameStartX = kMatrixStartX - kMinoWidth;
const int kFrameStartY = kMatrixStartY - kMinoHeight;
const int kFrameRows = kVisibleRows + 2;
const int kFrameCols = kVisibleCols + 2;
const int kSpace = 8;
