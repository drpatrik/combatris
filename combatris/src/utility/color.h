#pragma once

#include <SDL.h>

namespace utility {

enum class Color { None, Transparent, White, Blue, Red, Green, Black, Yellow, Cyan, Purple, Orange, Gray, SteelGray, Gold, Coral };

SDL_Color GetColor(Color color, uint8_t alpha = 255);

} // namespace utility
