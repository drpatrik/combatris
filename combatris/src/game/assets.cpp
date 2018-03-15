#include "game/assets.h"

#include <string>
#include <iostream>
#include <algorithm>

namespace {

#if defined(__linux__)
const std::string kAssetFolder = "assets/";
#else
const std::string kAssetFolder = "../../assets/";
#endif

void DeleteTexture(SDL_Texture* texture) {
  if (texture != nullptr) {
    SDL_DestroyTexture(texture);
  }
}

SDL_Texture* LoadTexture(SDL_Renderer *renderer, const std::string& name) {
  if (SDL_WasInit(SDL_INIT_EVERYTHING) == 0 || nullptr == renderer) {
    return nullptr;
  }
  auto full_path =  kAssetFolder + "art/" + name;
  auto surface = SDL_LoadBMP(full_path.c_str());

  if (nullptr == surface) {
    std::cout << "Failed to load surface " << full_path << " error : " << SDL_GetError() << std::endl;
    exit(-1);
  }
  auto texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_FreeSurface(surface);

  return texture;
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
  TetrominoAssetData(Tetromino::Type::Filler, Color::Black, kTetromino_No_Rotations, "Filler.bmp"),
  TetrominoAssetData(Tetromino::Type::Border, Color::Black, kTetromino_No_Rotations, "Border.bmp")
};

} // namespace

Assets::Assets(SDL_Renderer *renderer) {
  for (const auto &data : kTetrominoAssetData) {
    tetrominos_.push_back(std::make_shared<Tetromino>(
        renderer, data.type_, data.color_, data.rotations_,
        std::shared_ptr<SDL_Texture>(LoadTexture(renderer, data.image_name_), DeleteTexture)));
    alpha_textures_.push_back(std::shared_ptr<SDL_Texture>(LoadTexture(renderer, data.image_name_), DeleteTexture));
  }
  std::for_each(kAllFonts.begin(), kAllFonts.end(), [this](const auto& f) { fonts_.Get(f); });
}
