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

struct TetrominoAssetData {
  TetrominoAssetData(Tetromino::Type type, const std::string& image , const std::vector<TetrominoMetadata>& metadata) :
      type_(type), image_(image), metadata_(metadata) {}

  Tetromino::Type type_;
  std::string image_;
  std::vector<TetrominoMetadata> metadata_;
};

std::vector<TetrominoAssetData> kTetrominoAssetData {
  TetrominoAssetData(Tetromino::Type::I_Block, "I-Block.bmp", kTetrominoI_Blocks),
  TetrominoAssetData(Tetromino::Type::J_Block, "J-Block.bmp", kTetrominoJ_Blocks),
  TetrominoAssetData(Tetromino::Type::L_Block, "L-Block.bmp", kTetrominoL_Blocks),
  TetrominoAssetData(Tetromino::Type::O_Block, "O-Block.bmp", kTetrominoO_Blocks),
  TetrominoAssetData(Tetromino::Type::S_Block, "S-Block.bmp", kTetrominoS_Blocks),
  TetrominoAssetData(Tetromino::Type::T_Block, "T-Block.bmp", kTetrominoT_Blocks),
  TetrominoAssetData(Tetromino::Type::Z_Block, "Z-Block.bmp", kTetrominoZ_Blocks),
};

}

AssetManager::AssetManager(SDL_Renderer *renderer) {
  for (const auto asset : kTetrominoAssetData) {
    auto texture = std::shared_ptr<SDL_Texture>(LoadTexture(renderer, asset.image_), DeleteTexture);

    tetrominos_.push_back(std::make_shared<Tetromino>(renderer, asset.type_, asset.metadata_, texture));
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
