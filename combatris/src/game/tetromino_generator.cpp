#include "game/tetromino_generator.h"

namespace {

struct TetrominoAssetData {
  TetrominoAssetData(Tetromino::Type type, Color color, const std::vector<TetrominoRotationData>& rotations) :
      type_(type), color_(GetColor(color)), rotations_(rotations) {}

  Tetromino::Type type_;
  SDL_Color color_;
  std::vector<TetrominoRotationData> rotations_;
};

std::vector<TetrominoAssetData> kTetrominoAssetData {
  TetrominoAssetData(Tetromino::Type::I, Color::Cyan, kTetromino_I_Rotations),
  TetrominoAssetData(Tetromino::Type::J, Color::Blue, kTetromino_J_Rotations),
  TetrominoAssetData(Tetromino::Type::L, Color::Orange, kTetromino_L_Rotations),
  TetrominoAssetData(Tetromino::Type::O, Color::Yellow, kTetromino_O_Rotations),
  TetrominoAssetData(Tetromino::Type::S, Color::Green, kTetromino_S_Rotations),
  TetrominoAssetData(Tetromino::Type::T, Color::Purple, kTetromino_T_Rotations),
  TetrominoAssetData(Tetromino::Type::Z,  Color::Red, kTetromino_Z_Rotations),
};

}

TetrominoGenerator::TetrominoGenerator(SDL_Renderer *renderer, AssetManager& asset_manager) : renderer_(renderer), asset_manager_(asset_manager) {
  for (const auto& data : kTetrominoAssetData) {
    tetrominos_.push_back(std::make_shared<Tetromino>(renderer_,data.type_, data.color_, data.rotations_, asset_manager_.GetSprite(static_cast<int>(data.type_) - 1)));
  }
}

std::unique_ptr<TetrominoSprite> TetrominoGenerator::Get() {
  return Get(static_cast<Tetromino::Type>(distribution_(engine_)));
}

std::unique_ptr<TetrominoSprite> TetrominoGenerator::Get(Tetromino::Type type) {
  int type_as_int = static_cast<int>(type) - 1;

  return std::make_unique<TetrominoSprite>(*tetrominos_.at(type_as_int));
}
