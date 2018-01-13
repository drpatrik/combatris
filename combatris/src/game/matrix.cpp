#include "game/matrix.h"

#include <iomanip>

namespace {

void RenderGrid(SDL_Renderer* renderer) {
  const SDL_Color gray { 105, 105, 105, 255 };

  SDL_SetRenderDrawColor(renderer, gray.r, gray.g, gray.b, gray.a);
  SDL_Rect rc { kMatrixStartX, kMatrixStartY, kMatrixWidth, kMatrixHeight };
  SDL_RenderFillRect(renderer, &rc);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

  rc = { 0, kMatrixStartY + 1, kBlockWidth - 2, kBlockHeight - 2 };

  for (int row = 0; row < kVisibleRows; ++row) {
    rc.x = kMatrixStartX + 1;
    for (int col = 0; col < kVisibleCols; ++col) {
      SDL_RenderFillRect(renderer, &rc);
      rc.x += kBlockWidth;
    }
    rc.y += kBlockHeight;
  }
}

void SetupRows(Matrix::Type& matrix) {
  const int kEndRow = kRows - 1;

  for (int col = 0; col < kCols; ++col) {
    matrix.at(kEndRow).at(col) = kBorderSpriteID ;
  }
}

void SetupCols(Matrix::Type& matrix) {
  const int kEndCol = kCols - 1;

  for (int row = 0; row < kRows; ++row) {
    matrix.at(row).at(0) = kBorderSpriteID;
    matrix.at(row).at(1) = kBorderSpriteID;
    matrix.at(row).at(kEndCol - 1) = kBorderSpriteID ;
    matrix.at(row).at(kEndCol) = kBorderSpriteID ;
  }
}

}

void Matrix::Print() {
  for (int row = 0; row < kRows; ++row) {
    for (int col = 0; col < kCols; ++ col) {
      std::cout << std::setw(2) << matrix_.at(row).at(col);
      if (col < kCols -1 ) {
        std::cout << ", ";
      }
    }
    std::cout << std::endl;
  }
}

void Matrix::Initialize() {
  matrix_ = Type(kRows, std::vector<int>(kCols, 0));
  SetupRows(matrix_);
  SetupCols(matrix_);
  ingame_matrix_ = matrix_;
  // Test Data
  if (IsValid(Position(1, 0), tetrominos_.at(0)->GetRotationData(Tetromino::Angle::A0))) {
    Insert(Position(1, 2), tetrominos_.at(0)->GetRotationData(Tetromino::Angle::A0));
  } else {
    std::cout << "Not valid" << std::endl;
  }
}

void Matrix::Render() {
  RenderGrid(renderer_);
  for (int row = kVisibleRowStart; row < kVisibleRowEnd; ++row) {
    for (int col = kVisibleColStart; col < kVisibleColEnd; ++col) {
      int id = ingame_matrix_.at(row).at(col);

      if (0 == id) {
        continue;
      }
      const auto& tetromino = (id < 10) ? *tetrominos_.at(id - 1) : *tetrominos_.at(id - 10);
      Position pos(row_to_visible(row), col_to_visible(col));

      if (id < 10) {
        tetromino.Render(pos);
      } else {
        tetromino.RenderGhost(pos);
      }
    }
  }
}

bool Matrix::IsValid(const Position& pos, const TetrominoRotationData& rotation) {
  const auto& data = rotation.data_;

  for (size_t row = 0; row < data.size(); ++row) {
    for (size_t col  = 0; col < data.at(row).size(); ++col) {
      int try_row = pos.row() + row;
      int try_col = pos.col() + col;

      if (ingame_matrix_.at(try_row).at(try_col) != 0 && data.at(row).at(col) != 0) {
        ingame_matrix_ = matrix_;
        return false;
      }
      ingame_matrix_.at(try_row).at(try_col) = data.at(row).at(col);
    }
  }
  return true;
}

void Matrix::Insert(const Position& pos, const TetrominoRotationData& rotation) {
  const auto& data = rotation.data_;

  for (size_t row = 0; row < data.size(); ++row) {
    for (size_t col  = 0; col < data.at(row).size(); ++col) {
      int insert_row = pos.row() + row;
      int insert_col = pos.col() + col;

      if (data.at(row).at(col) == 0) {
        continue;
      }
      ingame_matrix_.at(insert_row).at(insert_col) = data.at(row).at(col);
    }
  }
}
