#pragma once

#include <cstddef>

const int kWidth = 755;
const int kHeight = 600;
const int kBlackAreaX = 268;
const int kBlackAreaY = 97;
const int kBlackAreadWidth = 461;
const int kBlackAreadHeight = 353;
const double kFPS = 60;
const int kGameTime = 180; // multiple of 60
const int kHurryUpTimeLimit = 10;
const int kNormalFontSize = 25;
const int kSmallFontSize = 15;
const int kLargeFontSize = 45;
const int kSpriteWidth = 38;
const int kSpriteHeight = 38;
const size_t kNumSprites = 5; // Red, Blue, Green, Yellow and Purple
const size_t kMatchNumber = 3;
const int kRows = 9;
const int kCols = 8;
const int kShowHintTimer = 10;
const int kIdlePenaltyTimer = 3;
const int kInitialThresholdStep = 1;
const int kThresholdMultiplier = 100;
const int kBoardStartX = 340;
const int kBoardEndX = kBoardStartX + (kCols * kSpriteWidth);
const int kBoardStartY = 95;
const int kBoardEndY = kBoardStartY + (kRows * kSpriteHeight);
const double kSpriteHeightTimes1_5 = kSpriteHeight * 1.5;
