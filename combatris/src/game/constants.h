#pragma once

const int kMenuHeight = 350;
const int kMaxNumberOfLevels = 15;
const int kVisibleRows = 20;
const int kVisibleCols = 10;
const int kMatrixFirstRow = 20;
const int kMatrixLastRow = kVisibleRows + kMatrixFirstRow;
const int kMatrixFirstCol = 2;
const int kMatrixLastCol = kVisibleCols + 2;
const int kSkylineOffset = 2;
const int kSkylineStartRow = kMatrixFirstRow - kSkylineOffset;
const int kMinoWidth = 32;
const int kMinoHeight = 32;
const int kMatrixWidth = (kVisibleCols * kMinoWidth);
const int kMatrixHeight = (kVisibleRows * kMinoHeight);
const int kBoxWidth = 220;
const int kBoxHeight = 220 + 4;
const int kSpaceBetweenBoxes = 15;
const int kMultiPlayerPaneWidth = (kBoxWidth * 2) + kSpaceBetweenBoxes;
const int kUpperBorderHeight = 60 + kMinoHeight;
const int kMultiPlayerPaneHeight = kMatrixHeight + (kMinoHeight * 2);
const int kMultiPlayerWidthAddOn =  kMultiPlayerPaneWidth + 24;
const int kWidth = ((kVisibleCols * kMinoWidth) + 400 + (kMinoWidth * 2));
const int kHeight = (kVisibleRows * kMinoHeight) + kUpperBorderHeight + (kMinoHeight * 2);
const int kMatrixStartX = 200 + kMinoWidth;
const int kMatrixEndX = kMatrixStartX + (kVisibleCols * kMinoWidth);
const int kMatrixStartY = kUpperBorderHeight;
const int kMatrixEndY = kMatrixStartY + (kVisibleRows * kMinoHeight);
const int kSpace = 8;
const int kBuffertVisible = 10;
// Campaign specific constants
const int kSprintGoal = 40;
const int kTimesUpSoon = 15;
const int kGameTimeBattle = 120;
const int kGameTimeUltra = 180;
