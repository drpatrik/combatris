#pragma once

#include <SDL.h>

enum class Color { White, Blue, Red, Green, Black, Yellow, Cyan, Purple, Orange, Gray, Score };

SDL_Color GetColor(Color color, uint8_t alpha = 255);
