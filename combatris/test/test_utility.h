#include "game/matrix.h"
#include "game/assets.h"

#if defined(_WIN64)
#pragma warning(disable:4101) // conversion from size_t to int
#endif

inline std::pair<std::shared_ptr<Assets>, std::shared_ptr<Matrix>> SetupTestHarness(const Matrix::Type& test_matrix) {
  auto assets = std::make_shared<Assets>(nullptr);
  auto matrix = std::make_shared<Matrix>(test_matrix, assets->GetTetrominos());

  return std::make_pair(assets, matrix);
}

inline std::shared_ptr<Matrix> SetupTestHarnessMatrixOnly(const Matrix::Type& test_matrix) {
  auto asset_manager = std::make_shared<Assets>(nullptr);
  auto matrix = std::make_shared<Matrix>(test_matrix, asset_manager->GetTetrominos());

  return matrix;
}
