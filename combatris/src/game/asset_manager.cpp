#include "asset_manager.h"
#include "tools/text.h"

#include <iostream>

namespace {

const std::string kAssetFolder = "../../assets/";

SDL_Texture* LoadTexture(SDL_Renderer *renderer, const std::string& name) {
  std::string full_path =  kAssetFolder + "art/" + name;

  SDL_Surface* surface = SDL_LoadBMP(full_path.c_str());
  if (nullptr == surface) {
    std::cout << "Failed to load surface " << full_path << " error : " << SDL_GetError() << std::endl;
    exit(-1);
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_FreeSurface(surface);

  return texture;
}

TTF_Font *LoadFont(const std::string& name, int size) {
  std::string full_path = kAssetFolder + "fonts/" + name;

  TTF_Font *font = TTF_OpenFont(full_path.c_str(), size);

  if (font == nullptr) {
    std::cout << "Failed to load text " << full_path << " error : " << SDL_GetError() << std::endl;
    exit(-1);
  }

  return font;
}

void DeleteTexture(SDL_Texture* texture) {
  if (texture != nullptr) {
    SDL_DestroyTexture(texture);
  }
}

}

AssetManager::AssetManager(SDL_Renderer *renderer) {
  std::vector<Tetromino::Type> types {  Tetromino::Type::I_Block, Tetromino::Type::J_Block, Tetromino::Type::L_Block, Tetromino::Type::O_Block, Tetromino::Type::S_Block, Tetromino::Type::T_Block, Tetromino::Type::Z_Block };
  std::vector<std::string> tetrominos { "I-Block.bmp", "J-Block.bmp", "L-Block.bmp", "O-Block.bmp", "S-Block.bmp", "T-Block.bmp", "Z-Block.bmp" };

  for (size_t i = 0; i < tetrominos.size(); ++i) {
    auto texture = std::shared_ptr<SDL_Texture>(LoadTexture(renderer, tetrominos[i]), DeleteTexture);

    sprites_.push_back(std::make_shared<Tetromino>(renderer, types[i], texture));
  }
  std::vector<std::pair<std::string, int>> fonts {
    std::make_pair("Cabin-Regular.ttf", kNormalFontSize),
    std::make_pair("Cabin-Bold.ttf", kNormalFontSize),
    std::make_pair("Cabin-Regular.ttf", kSmallFontSize),
    std::make_pair("Cabin-Bold.ttf", kLargeFontSize)
  };

  for (const auto& f:fonts) {
    fonts_.push_back(UniqueFontPtr{ LoadFont(f.first, f.second) });
  }
}
