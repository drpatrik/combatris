#include "game/matrix.h"
#include "game/tetromino_tspin_detection.h"

#include <random>
#include <iomanip>

namespace {
std::mt19937 kGenerator{ std::random_device{}() };
std::uniform_int_distribution<size_t> kDistribution(0, kVisibleCols - 1);

const int kEmptyID = static_cast<int>(Tetromino::Type::Empty);
const int kBombID = static_cast<int>(Tetromino::Type::Bomb);
const int kBorderID = static_cast<int>(Tetromino::Type::Border);
const int kSolidID = static_cast<int>(Tetromino::Type::Solid);
const int kGhostAddOn = kBorderID + 1;
const std::vector<int> kEmptyRow = { kBorderID, kBorderID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kBorderID, kBorderID };
const std::vector<int> kSolidRow = { kBorderID, kBorderID, kSolidID, kSolidID, kSolidID, kSolidID,  kSolidID,
                                    kSolidID,  kSolidID,  kSolidID, kSolidID, kSolidID, kBorderID, kBorderID };

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

  rc = { 0, kMatrixStartY + 1, kMinoWidth - 2, kMinoHeight - 2 };

  for (int row = 0; row < kVisibleRows; ++row) {
    rc.x = kMatrixStartX + 1;
    for (int col = 0; col < kVisibleCols; ++col) {
      SDL_RenderFillRect(renderer, &rc);
      rc.x += kMinoWidth;
    }
    rc.y += kMinoHeight;
  }
}

void SetupPlayableArea(Matrix::Type& matrix) {
  for (int row = 0; row < kVisibleRowEnd; ++row) {
    matrix[row] = kEmptyRow;
  }
}

Lines RemoveLinesCleared(Matrix::Type& matrix) {
  Lines lines;

  for (int row = kVisibleRowStart; row < kVisibleRowEnd; ++row) {
    const auto& line = matrix[row];

    if (line[kVisibleColStart] == kEmptyID) {
      continue;
    }
    if (std::find_if(line.begin(), line.end(), [](auto elem) { return elem == kEmptyID || elem == kBombID; }) == line.end()) {
      lines.push_back(Line(row, line));
      matrix[row] = kEmptyRow;
    }
  }
  return lines;
}

void MoveLineDown(int end_row, Matrix::Type& matrix) {
  Matrix::Type tmp;

  std::copy(matrix.begin(), matrix.begin() + end_row, std::back_inserter(tmp));
  std::copy(tmp.begin(), tmp.end(), matrix.begin() + 1);
  matrix[0] = kEmptyRow;
}

void CollapseMatrix(const Lines& lines_cleared, Matrix::Type& matrix) {
  for (const auto& line : lines_cleared) {
    MoveLineDown(line.row_, matrix);
  }
}

bool DetectPerfectClear(const Matrix::Type& matrix) {
  return matrix[matrix.size() - 3] == kEmptyRow;
}

int MoveLinesUp(int lines, Matrix::Type& matrix) {
  int first_non_empty_row = 0;

  for (int row = 0; row < kVisibleRowEnd; ++row) {
    first_non_empty_row = row;
    if (matrix[row] != kEmptyRow) {
      break;
    }
  }
  if (first_non_empty_row <= 0) {
    return 0;
  }
  Matrix::Type tmp;

  std::copy(matrix.begin() + first_non_empty_row, matrix.end() - 2, std::back_inserter(tmp));

  lines = std::min(lines, first_non_empty_row);

  if (lines > 0) {
    std::copy(tmp.begin(), tmp.end(), matrix.end() - 2 - lines - tmp.size());
  }
  return lines;
}

void InsertSolidLines(int lines, Matrix::Type& matrix) {
  int n = kDistribution(kGenerator);

  for (int l = lines - 1; l >= 0; --l) {
    matrix.at(kVisibleRowEnd - l - 1) = kSolidRow;
    if (l % 2 == 0) {
      n = kDistribution(kGenerator);
    }
    matrix.at(kVisibleRowEnd - l - 1).at(kVisibleRowStart + n) = kBombID;
  }
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

bool Matrix::InsertLines(int lines) {
  lines = MoveLinesUp(lines, master_matrix_);

  if (lines <= 0) {
    return false;
  }

  InsertSolidLines(lines, master_matrix_);
  matrix_ = master_matrix_;

  return true;
}

void Matrix::RemoveLines() {
  Lines lines;

  for (int row = 0; row < kVisibleRowEnd; ++row) {
    auto& line = master_matrix_[row];

    if (std::count(line.begin() + 2, line.end() - 2, kSolidID) >= kVisibleCols - 1) {
      lines.push_back(Line(row, line));
    }
  }
  CollapseMatrix(lines, master_matrix_);
  matrix_ = master_matrix_;
}

bool Matrix::IsValid(const Position& pos, const TetrominoRotationData& rotation_data) const {
  if (pos.col() < 0 || pos.row() < 0) {
    return false;
  }
  const auto& shape = rotation_data.shape_;

  for (size_t row = 0; row < shape.size(); ++row) {
    for (size_t col  = 0; col < shape[row].size(); ++col) {
      const auto elem = master_matrix_[pos.row() + row][pos.col() + col];

      if (elem != kEmptyID && elem != kBombID && shape[row][col] != kEmptyID) {
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

Matrix::CommitReturnType Matrix::Commit(Tetromino::Type type, Tetromino::Move latest_move, const Position& current_pos, const TetrominoRotationData& rotation_data) {
  TSpinType tspin_type = TSpinType::None;

  auto pos = GetDropPosition(current_pos, rotation_data);

  Insert(master_matrix_, pos, rotation_data);

  if (Tetromino::Type::T == type && latest_move == Tetromino::Move::Rotation) {
    tspin_type = DetectTSpin(master_matrix_, pos, rotation_data.angle_index_);
  }

  auto lines_cleared = RemoveLinesCleared(master_matrix_);

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
