#include "utility/fonts.h"

#include <iostream>

namespace {

const std::string kAssetFolder = "../../assets/";

const std::unordered_map<Font::Typeface, const std::unordered_map<Font::Emphasis, std::string>> kFonts = {
  { Font::Typeface::Cabin, { { Font::Emphasis::Normal, "Cabin-Regular.ttf" }, { Font::Emphasis::Bold, "Cabin-Bold.ttf" } } },
  { Font::Typeface::ObelixPro, { { Font::Emphasis::Normal, "Cabin-Regular.ttf" } } }
};

TTF_Font *LoadFont(const std::string& name, int size) {
  if (TTF_WasInit() == 0) {
    return nullptr;
  }
  auto full_path = kAssetFolder + "fonts/" + name;
  auto font = TTF_OpenFont(full_path.c_str(), size);

  if (nullptr == font) {
    std::cout << "Failed to load font " << full_path << " error : " << SDL_GetError() << std::endl;
    exit(-1);
  }

  return font;
}

}  // namespace

TTF_Font* Fonts::Get(const Font& font) const {
  if (font_cache_.count(font)) {
    return font_cache_.at(font).get();
  }
  std::string file_name;

  try {
     file_name = kFonts.at(font.typeface_).at(font.emphasis_);
  } catch (std::out_of_range) {
    std::cout << "Font: \"" + ToString(font.typeface_) + "\" \"" + ToString(font.emphasis_) + "\" not found" << std::endl;
    exit(-1);
  }
  auto font_ptr = std::shared_ptr<TTF_Font>(LoadFont(file_name, font.size_), TTF_CloseFont);

  font_cache_.insert(std::make_pair(font, font_ptr));

  return font_ptr.get();
}
