#pragma once

#include <SDL.h>

#include <array>
#include <cassert>

namespace utility {

enum Color { None, Transparent, White, Blue, Red, Green, Black, Yellow, Cyan, Purple, Orange, Gray, SteelGray, Gold, Coral, LastColor };

const std::array<SDL_Color, static_cast<size_t>(Color::LastColor)> kColors {{
  { 0, 0, 0, 0 }, // None}
  { 0, 0, 1, 0 }, // Transparent
  { 255, 255, 255, 0 }, // White
  { 0, 0, 240, 0 }, // Blue
  { 240, 0, 0, 0 }, // Red
  { 0, 240, 0, 0 }, // Green
  { 0, 0, 0, 0 }, // Black
  { 240, 240, 0, 0 }, // Yellow
  { 0, 240, 240, 0 }, // Cyan
  { 160, 0, 240, 0 }, // Purple
  { 240, 160, 0, 0 }, // Orange
  { 102, 102, 102, 0 }, // Gray
  { 182, 195, 201, 0 }, // Steel Gray
  { 255, 215, 0, 0 }, // Gold
  { 255,127,80, 0 } // Coral
  }};

inline const SDL_Color GetColor(Color color, uint8_t alpha = 255) {
  assert(color != Color::None && color != Color::LastColor);
  return { kColors[color].r, kColors[color].g, kColors[color].b, alpha };
}

} // namespace utility
