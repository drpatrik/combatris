#include "utility/color.h"

namespace utility {

SDL_Color GetColor(Color color, uint8_t alpha) {
  if (Color::SteelGray == color) {
    return { 182, 195, 201, alpha };
  } else if (Color::Blue == color) {
    return { 0, 0, 240, alpha };
  } else if (Color::Red == color) {
    return { 240, 0, 0, alpha };
  } else if (Color::Green == color) {
    return  { 0, 240, 0, alpha };
  } else if (Color::Black == color) {
    return  { 0, 0, 0, alpha };
  } else if (Color::Yellow == color) {
    return { 240, 240, 0, alpha };
  } else if (Color::Cyan == color) {
    return { 0, 240, 240, alpha };
  } else if (Color::Orange == color) {
    return { 240, 160, 0, alpha };
  } else if (Color::Purple == color) {
    return { 160, 0, 240, alpha };
  } else if (Color::Gray == color) {
    return { 102, 102, 102, alpha };
  } else if (Color::Gold == color) {
    return { 255, 215, 0, alpha };
  } else if (Color::Coral == color) {
    return { 255,127,80, alpha };
  } else if (Color::Transparent == color) {
    return { 0, 0, 1, alpha };
  } else {
    return { 255, 255, 255, alpha };
  }
}

} // namespace utility
