#pragma once

#include <SDL_ttf.h>
#include <unordered_map>
#include <string>
#include <memory>

namespace utility {

struct Font {
  enum class Typeface { Cabin, ObelixPro };
  enum class Emphasis { Normal, Bold };

  Font(Typeface typeface, Emphasis emphasis, int size) : typeface_(typeface), emphasis_(emphasis), size_(size) {}

  Typeface typeface_;
  Emphasis emphasis_;
  int size_;
};

inline bool operator==(const Font& lhs, const Font& rhs) {
  return lhs.typeface_ == rhs.typeface_ && lhs.emphasis_ == rhs.emphasis_ && lhs.size_ == rhs.size_;
}

inline std::string ToString(Font::Typeface typeface) {
  switch (typeface) {
    case Font::Typeface::Cabin:
      return "Cabin";
    case Font::Typeface::ObelixPro:
      return "ObelixPro";
  }
  return "";
}

inline std::string ToString(Font::Emphasis emphasis) {
  switch (emphasis) {
    case Font::Emphasis::Normal:
      return "Normal";
    case Font::Emphasis::Bold:
      return "Bold";
  }
  return "";
}

} // namespace utility

namespace std {

using namespace utility;

template<> struct hash<Font> {
  size_t operator()(const Font& f) const noexcept {
    std::string key = std::to_string(static_cast<int>(f.typeface_)) + std::to_string(static_cast<int>(f.emphasis_)) + std::to_string(f.size_);

    return std::hash<std::string>{}(std::move(key));
  }
};

} // namespace std;

namespace utility {

class Fonts final {
 public:
  Fonts() {};

  ~Fonts() noexcept {};

  Fonts(const Fonts&) = delete;

  TTF_Font* Get(const Font& font) const;

  TTF_Font* Get(Font::Typeface typeface, Font::Emphasis emphasis, int size) const { return Get(Font(typeface, emphasis, size)); }

 private:
  mutable std::unordered_map<Font, std::shared_ptr<TTF_Font>> font_cache_;
};

} // namespace utility
