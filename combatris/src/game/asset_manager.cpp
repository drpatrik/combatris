#include "asset_manager.h"

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
  std::vector<SpriteID> sprite_ids {  SpriteID::I_Block, SpriteID::J_Block, SpriteID::L_Block, SpriteID::O_Block, SpriteID::S_Block, SpriteID::T_Block, SpriteID::Z_Block };
  std::vector<std::string> sprites { "I-Block.bmp", "J-Block.bmp", "L-Block.bmp", "O-Block.bmp", "S-Block.bmp", "T-Block.bmp", "Z-Block.bmp" };

  for (size_t i = 0; i < sprites.size(); ++i) {
    auto texture = std::shared_ptr<SDL_Texture>(LoadTexture(renderer, sprites[i]), DeleteTexture);

    sprites_.push_back(std::make_shared<Sprite>(sprite_ids[i], texture));
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
