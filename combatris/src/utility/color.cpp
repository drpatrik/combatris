#include "utility/color.h"

namespace utility {

SDL_Color GetColor(Color color, uint8_t alpha) {
  if (color == Color::SteelGray) {
    return { 182, 195, 201, alpha };
  } else if (color == Color::Blue) {
    return { 0, 0, 240, alpha };
  } else if (color == Color::Red) {
    return { 240, 0, 0, alpha };
  } else if (color == Color::Green) {
    return  { 0, 240, 0, alpha };
  } else if (color == Color::Black) {
    return  { 0, 0, 0, alpha };
  } else if (color == Color::Yellow) {
    return { 240, 240, 0, alpha };
  } else if (color == Color::Cyan) {
    return { 0, 240, 240, alpha };
  } else if (color == Color::Orange) {
    return { 240, 160, 0, alpha };
  } else if (color == Color::Purple) {
    return { 160, 0, 240, alpha };
  } else if (color == Color::Gray) {
    return { 102, 102, 102, alpha };
  } else if (color == Color::Gold) {
    return { 255, 215, 0, alpha };
  } else if (color == Color::Coral) {
    return { 255,127,80, alpha };
  } else {
    return { 255, 255, 255, alpha };
  }
}

} // namespace utility
