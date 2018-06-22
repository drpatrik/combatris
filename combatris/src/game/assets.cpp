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

SDL_Texture* LoadTexture(SDL_Renderer *renderer, const std::string& name, Color transparent_color = Color::None) {
  if (SDL_WasInit(SDL_INIT_EVERYTHING) == 0 || nullptr == renderer) {
    return nullptr;
  }
  auto full_path =  kAssetFolder + "art/" + name;
  auto surface = SDL_LoadBMP(full_path.c_str());

  if (nullptr == surface) {
    std::cout << "Failed to load surface " << full_path << " error : " << SDL_GetError() << std::endl;
    exit(-1);
  }
  if (Color::Transparent == transparent_color) {
    const auto c = GetColor(transparent_color);

    SDL_SetColorKey(surface, 1, SDL_MapRGB(surface->format, c.r, c.g, c.b));
  }
  auto texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_FreeSurface(surface);

  return texture;
}

SDL_Texture* LoadTexture(SDL_Renderer *renderer, const std::string& name, int i, Color transparent_color = Color::None) {
  return LoadTexture(renderer, name + "_" + std::to_string(i) + ".bmp", transparent_color);
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
  TetrominoAssetData(Tetromino::Type::Solid, Color::Black, kTetromino_No_Rotations, "Border.bmp"),
  TetrominoAssetData(Tetromino::Type::Bomb, Color::Black, kTetromino_No_Rotations, "Filler.bmp"),
  TetrominoAssetData(Tetromino::Type::Border, Color::Black, kTetromino_No_Rotations, "Border.bmp")
};

struct TextureAssetData {
  TextureAssetData(const std::string& name, Color transparent_color)
      : name_(name), transparent_color_(transparent_color) {}

  std::string name_;
  Color transparent_color_;
};

std::vector<TextureAssetData> kTextures {
  { "Checkmark.bmp", Color::Transparent },
  { "Circle.bmp", Color::Transparent }
};

} // namespace

Assets::Assets(SDL_Renderer *renderer) : fonts_(std::make_shared<Fonts>()) {
  for (const auto& data : kTetrominoAssetData) {
    tetrominos_.push_back(std::make_shared<Tetromino>(
        renderer, data.type_, data.color_, data.rotations_,
        std::shared_ptr<SDL_Texture>(LoadTexture(renderer, data.image_name_), DeleteTexture)));
    alpha_textures_.push_back(std::shared_ptr<SDL_Texture>(LoadTexture(renderer, data.image_name_), DeleteTexture));
  }
  for (const auto& data : kTextures) {
    textures_.push_back(std::shared_ptr<SDL_Texture>(LoadTexture(renderer, data.name_, data.transparent_color_), DeleteTexture));
  }
  for (int i = 1; i <=24; ++i) {
    hourglass_textures_.push_back(std::shared_ptr<SDL_Texture>(LoadTexture(renderer, "Hourglass", i), DeleteTexture));
  }
  std::for_each(kFontsToPreload.begin(), kFontsToPreload.end(), [this](const auto& f) { fonts_->Get(f); });
}

std::tuple<std::shared_ptr<SDL_Texture>, int, int> Assets::GetTexture(Type type) const {
  auto texture = textures_.at(static_cast<int>(type));
  int w, h;

  SDL_QueryTexture(texture.get(), nullptr, nullptr, &w, &h);

  return std::make_tuple(texture, w, h);
}
