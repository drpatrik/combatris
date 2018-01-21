#include "game/matrix.h"

#include <iomanip>

namespace {

const int kGhostAddOn = 10;
const int kBorderSpriteID = static_cast<int>(Tetromino::Type::Z) + 1;

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

void SetupPlayableArea(Matrix::Type& matrix) {
  for (int row = 0; row < kVisibleRowEnd; ++row) {
    auto& line = matrix.at(row);

    std::fill(line.begin() + kVisibleColStart, line.end() - kVisibleColStart, 0);
  }
}

Matrix::Lines RemoveClearedLines(Matrix::Type& matrix) {
  Matrix::Lines lines;

  for (int row = kVisibleRowStart; row < kVisibleRowEnd; ++row) {
    auto& line = matrix.at(row);

    if (line.at(kVisibleColStart) == 0) {
      continue;
    }
    if (std::find(line.begin(), line.end(), 0) == line.end()) {
      lines.push_back(Line(row, line));
      std::fill(line.begin() + kVisibleColStart, line.end() - kVisibleColStart, 0);
    }
  }
  return lines;
}

void CollapseMatrix(const Matrix::Lines& cleared_lines, Matrix::Type& matrix) {
  auto const& last_cleared_line = cleared_lines.rbegin()->row_in_matrix_;

  for (size_t i = 0; i < cleared_lines.size(); ++i) {
    for (int row = last_cleared_line; row > kVisibleRowStart; --row) {
      std::swap(matrix.at(row), matrix.at(row - 1));
    }
  }
}

}

void Matrix::Print() {
  for (int row = 0; row < static_cast<int>(master_matrix_.size()); ++row) {
    for (int col = 0; col < static_cast<int>(master_matrix_.at(row).size()); ++ col) {
      std::cout << std::setw(2) << master_matrix_.at(row).at(col);
      if (col < kCols -1 ) {
        std::cout << ", ";
      }
    }
    std::cout << std::endl;
  }
}

void Matrix::Initialize() {
  master_matrix_ = Matrix::Type(kRows + 1, std::vector<int>(kCols, kBorderSpriteID));

  SetupPlayableArea(master_matrix_);
  ingame_matrix_ = master_matrix_;
}

void Matrix::Render() {
  RenderGrid(renderer_);
  for (int row = kVisibleRowStart; row < kVisibleRowEnd; ++row) {
    for (int col = kVisibleColStart; col < kVisibleColEnd; ++col) {
      int id = ingame_matrix_.at(row).at(col);

      if (0 == id) {
        continue;
      }
      const auto& tetromino = (id < kGhostAddOn) ? *tetrominos_.at(id - 1) : *tetrominos_.at(id - kGhostAddOn - 1);
      Position pos(row_to_visible(row), col_to_visible(col));

      if (id < kGhostAddOn) {
        tetromino.Render(pos);
      } else {
        tetromino.RenderGhost(pos);
      }
    }
  }
}

bool Matrix::IsValid(const Position& pos, const TetrominoRotationData& rotation_data) const {
  const auto& shape = rotation_data.shape_;

  for (size_t row = 0; row < shape.size(); ++row) {
    for (size_t col  = 0; col < shape.at(row).size(); ++col) {
      int try_row = pos.row() + row;
      int try_col = pos.col() + col;

      if (master_matrix_.at(try_row).at(try_col) != 0 && shape.at(row).at(col) != 0) {
        return false;
      }
    }
  }
  return true;
}

void Matrix::Insert(Type& matrix, const Position& pos, const TetrominoRotationData& rotation_data, bool insert_ghost) {
  const int ghost_add_on = (insert_ghost) ? kGhostAddOn : 0;
  const auto& shape = rotation_data.shape_;

  for (size_t row = 0; row < shape.size(); ++row) {
    for (size_t col  = 0; col < shape.at(row).size(); ++col) {
      int insert_row = pos.row() + row;
      int insert_col = pos.col() + col;

      if (shape.at(row).at(col) == 0) {
        continue;
      }
      matrix.at(insert_row).at(insert_col) = shape.at(row).at(col) + ghost_add_on;
    }
  }
}

Matrix::Lines Matrix::Commit(const Position& current_pos, const TetrominoRotationData& rotation_data) {
  auto pos = GetDropPosition(current_pos, rotation_data);

  Insert(master_matrix_, pos, rotation_data);

  auto lines_cleared = RemoveClearedLines(master_matrix_);

  CollapseMatrix(lines_cleared, master_matrix_);

  return lines_cleared;
}

Position Matrix::GetDropPosition(const Position& current_pos, const TetrominoRotationData& rotation_data) const {
  Position pos(current_pos);

  while (IsValid(Position(pos.row() + 1, pos.col()), rotation_data)) {
    pos.inc_row();
  }

  return pos;
}
