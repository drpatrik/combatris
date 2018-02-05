#pragma once

#include "game/matrix.h"

Event::BonusMove DetectTSpin(const Matrix::Type& matrix, const Position& pos, int angle_index);
