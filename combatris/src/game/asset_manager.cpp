#include "asset_manager.h"

#include <string>
#include <iostream>

namespace {

const std::string kAssetFolder = "../../assets/";

SDL_Texture* LoadTexture(SDL_Renderer *renderer, const std::string& name) {
  if (renderer == nullptr) {
    return nullptr;
  }
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
  TetrominoAssetData(Tetromino::Type type, Color color, const std::vector<TetrominoRotationData>& rotations, const std::string& image_name) :
      type_(type), color_(GetColor(color)), rotations_(rotations), image_name_(image_name) {}

  Tetromino::Type type_;
  SDL_Color color_;
  std::vector<TetrominoRotationData> rotations_;
  std::string image_name_;
};

std::vector<TetrominoAssetData> kTetrominoAssetData {
  TetrominoAssetData(Tetromino::Type::I, Color::Cyan, kTetromino_I_Rotations, "I.bmp"),
  TetrominoAssetData(Tetromino::Type::J, Color::Blue, kTetromino_J_Rotations, "J.bmp"),
  TetrominoAssetData(Tetromino::Type::L, Color::Orange, kTetromino_L_Rotations, "L.bmp"),
  TetrominoAssetData(Tetromino::Type::O, Color::Yellow, kTetromino_O_Rotations, "O.bmp"),
  TetrominoAssetData(Tetromino::Type::S, Color::Green, kTetromino_S_Rotations, "S.bmp"),
  TetrominoAssetData(Tetromino::Type::T, Color::Purple, kTetromino_T_Rotations, "T.bmp"),
  TetrominoAssetData(Tetromino::Type::Z, Color::Red, kTetromino_Z_Rotations, "Z.bmp"),
  TetrominoAssetData(Tetromino::Type::B, Color::Black, kTetromino_B_Rotations, "B.bmp"),
};

} // namespace

AssetManager::AssetManager(SDL_Renderer *renderer) {
  border_texture_ = std::shared_ptr<SDL_Texture>(LoadTexture(renderer, "Border.bmp"), DeleteTexture);

  for (const auto &data : kTetrominoAssetData) {
    tetrominos_.push_back(std::make_shared<Tetromino>(
        renderer, data.type_, data.color_, data.rotations_,
        std::shared_ptr<SDL_Texture>(LoadTexture(renderer, data.image_name_), DeleteTexture)));
  }

  if (renderer != nullptr) {
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
}
