#pragma once

#include <SDL.h>

namespace utility {

enum class Color { White, Blue, Red, Green, Black, Yellow, Cyan, Purple, Orange, Gray, SteelGray };

SDL_Color GetColor(Color color, uint8_t alpha = 255);

} // namespace utility
