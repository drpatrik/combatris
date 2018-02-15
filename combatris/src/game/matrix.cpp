#include "game/matrix.h"
#include "game/tetromino_tspin_detection.h"

#include <iomanip>

namespace {

const int kEmptyID =  static_cast<int>(Tetromino::Type::Empty);
const int kBorderID = static_cast<int>(Tetromino::Type::Border);
const int kGhostAddOn = kBorderID + 1;
const std::vector<int> kEmptyRow = { kBorderID, kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID, kBorderID };

void Print(const Matrix::Type& matrix) {
  for (int row = 0; row < static_cast<int>(matrix.size()); ++row) {
    for (int col = 0; col < static_cast<int>(matrix.at(row).size()); ++ col) {
      std::cout << std::setw(2) <<matrix.at(row).at(col);
      if (col < kCols -1 ) {
        std::cout << ", ";
      }
    }
    std::cout << std::endl;
  }
}

void RenderGrid(SDL_Renderer* renderer) {
  const SDL_Color gray { 51, 55, 66, 255 };

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
    matrix[row] = kEmptyRow;
  }
}

Lines RemoveClearedLines(Matrix::Type& matrix) {
  Lines lines;

  for (int row = kVisibleRowStart; row < kVisibleRowEnd; ++row) {
    const auto& line = matrix[row];

    if (line[kVisibleColStart] == kEmptyID) {
      continue;
    }
    if (std::find(line.begin(), line.end(), kEmptyID) == line.end()) {
      lines.push_back(Line(row, line));
      matrix[row] = kEmptyRow;
    }
  }
  return lines;
}

void MoveBlockDown(int end_row, Matrix::Type& matrix) {
  Matrix::Type tmp;

  std::copy(matrix.begin() + kVisibleRowStart, matrix.begin() + end_row, std::back_inserter(tmp));
  std::copy(tmp.begin(), tmp.end(), matrix.begin() + kVisibleRowStart + 1);
  matrix[kVisibleRowStart] = kEmptyRow;
}

void CollapseMatrix(const Lines& lines_cleared, Matrix::Type& matrix) {
  for (const auto& line : lines_cleared) {
    MoveBlockDown(line.row_, matrix);
  }
}

bool DetectPerfectClear(const Matrix::Type& matrix) {
  return matrix[matrix.size() - 3] == kEmptyRow;
}

} // namespace

void Matrix::Print(bool master) const { ::Print((master) ? master_matrix_ : matrix_); }

void Matrix::Initialize() {
  master_matrix_ = Matrix::Type(kRows + 1, std::vector<int>(kCols, kBorderID));

  SetupPlayableArea(master_matrix_);
  matrix_ = master_matrix_;
}

void Matrix::Render(double) {
  RenderGrid(renderer_);
  for (int col = kVisibleColStart - 1; col < kVisibleColEnd + 1; ++col) {
    tetrominos_[kBorderID - 1]->Render(Position(row_to_visible(kVisibleRowStart - 1), col_to_visible(col)));
  }
  for (int row = kVisibleRowStart; row < kVisibleRowEnd + 1; ++row) {
    for (int col = kVisibleColStart - 1; col < kVisibleColEnd + 1; ++col) {
      const int id = matrix_[row][col];

      if (kEmptyID == id) {
        continue;
      }
      const auto& tetromino = (id < kGhostAddOn) ? *tetrominos_[id - 1] : *tetrominos_[id - kGhostAddOn - 1];
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
  if (pos.col() < 0 || pos.row() < 0) {
    return false;
  }
  const auto& shape = rotation_data.shape_;

  for (size_t row = 0; row < shape.size(); ++row) {
    for (size_t col  = 0; col < shape[row].size(); ++col) {
      auto try_row = pos.row() + row;
      auto try_col = pos.col() + col;

      if (master_matrix_[try_row][try_col] != kEmptyID && shape[row][col] != kEmptyID) {
        return false;
      }
    }
  }
  return true;
}

void Matrix::Insert(Type& matrix, const Position& pos, const TetrominoRotationData& rotation_data, bool insert_ghost) {
  const auto ghost_add_on = (insert_ghost) ? kGhostAddOn : 0;
  const auto& shape = rotation_data.shape_;

  for (size_t row = 0; row < shape.size(); ++row) {
    for (size_t col  = 0; col < shape[row].size(); ++col) {
      auto insert_row = pos.row() + row;
      auto insert_col = pos.col() + col;

      if (shape[row][col] == kEmptyID) {
        continue;
      }
      matrix[insert_row][insert_col] = shape[row][col] + ghost_add_on;
    }
  }
}

Matrix::CommitReturnTyoe Matrix::Commit(Tetromino::Type type, Tetromino::Move latest_move, const Position& current_pos, const TetrominoRotationData& rotation_data) {
  TSpinType tspin_type = TSpinType::None;

  auto pos = GetDropPosition(current_pos, rotation_data);

  Insert(master_matrix_, pos, rotation_data);

  if (Tetromino::Type::T == type && latest_move == Tetromino::Move::Rotation) {
    tspin_type = DetectTSpin(master_matrix_, pos, rotation_data.angle_index_);
  }

  auto lines_cleared = RemoveClearedLines(master_matrix_);

  CollapseMatrix(lines_cleared, master_matrix_);

  auto perfect_clear = (lines_cleared.size() > 0 && DetectPerfectClear(master_matrix_));

  return std::make_tuple(lines_cleared, tspin_type, perfect_clear);
}

Position Matrix::GetDropPosition(const Position& current_pos, const TetrominoRotationData& rotation_data) const {
  Position pos(current_pos);

  while (IsValid(Position(pos.row() + 1, pos.col()), rotation_data)) {
    pos.inc_row();
  }

  return pos;
}
