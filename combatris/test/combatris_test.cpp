#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "game/matrix.h"

#include "catch.hpp"

namespace {

std::pair<std::shared_ptr<AssetManager>, std::shared_ptr<Matrix>> SetupTestHarness(const Matrix::Type& test_matrix) {
  auto asset_manager = std::make_shared<AssetManager>(nullptr);
  auto matrix = std::make_shared<Matrix>(test_matrix, asset_manager->GetTetrominos());

  return make_pair(asset_manager, matrix);
}

} // namespace

const std::vector<std::vector<int>> kClearTopRowBefore {
  {0, 0, 0, 0, 1, 1, 1, 1, 1, 1}, // 01
  {5, 5, 5, 5, 5, 5, 0, 0, 0, 0}, // 02
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 03
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 04
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 05
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 06
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 07
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 08
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 09
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 10
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 11
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 12
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 13
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 14
  {0, 0, 1, 1, 1, 1, 1, 1, 0, 0}, // 15
  {0, 0, 1, 1, 1, 1, 1, 1, 0, 0}, // 16
  {0, 0, 0, 1, 1, 1, 1, 0, 0, 0}, // 17
  {1, 1, 0, 1, 1, 1, 1, 0, 1, 1}, // 18
  {1, 0, 0, 1, 1, 1, 1, 0, 0, 1}, // 19
  {1, 1, 0, 1, 1, 1, 1, 0, 1, 1}  // 20
};

const std::vector<std::vector<int>> kClearTopRowAfter {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 01
  {5, 5, 5, 5, 5, 5, 0, 0, 0, 0}, // 02
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 03
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 04
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 05
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 06
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 07
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 08
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 09
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 10
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 11
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 12
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 13
  {0, 0, 0, 0, 0, 5, 0, 0, 0, 0}, // 14
  {0, 0, 1, 1, 1, 1, 1, 1, 0, 0}, // 15
  {0, 0, 1, 1, 1, 1, 1, 1, 0, 0}, // 16
  {0, 0, 0, 1, 1, 1, 1, 0, 0, 0}, // 17
  {1, 1, 0, 1, 1, 1, 1, 0, 1, 1}, // 18
  {1, 0, 0, 1, 1, 1, 1, 0, 0, 1}, // 19
  {1, 1, 0, 1, 1, 1, 1, 0, 1, 1}  // 20
};

TEST_CASE("ClearLinesAtTop") {
  auto [asset_manager, matrix] = SetupTestHarness(kClearTopRowBefore);

  auto tetrominos = asset_manager->GetTetrominos();

  Position insert_pos(0, kVisibleColStart);

  auto r = matrix->Commit(insert_pos, tetrominos.at(static_cast<int>(Tetromino::Type::I) - 1)->GetRotationData(Tetromino::Angle::A0));

  REQUIRE(r.size() == 1);
  REQUIRE(*matrix == kClearTopRowAfter);
}

const std::vector<std::vector<int>> kClearedLineWithGarbageBetweenBefore {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 01
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 02
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 03
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 04
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 05
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 06
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 07
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 08
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 09
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 11
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 12
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 13
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 14
  {1, 0, 1, 1, 1, 1, 1, 1, 0, 0}, // 15
  {1, 0, 1, 1, 1, 1, 1, 1, 1, 1}, // 16
  {1, 0, 0, 1, 1, 1, 1, 0, 0, 0}, // 17
  {1, 0, 0, 1, 1, 1, 1, 0, 1, 1}, // 18
  {1, 0, 1, 1, 1, 1, 1, 1, 1, 1}, // 19
  {1, 1, 0, 1, 1, 1, 1, 0, 1, 1}  // 20
};

const std::vector<std::vector<int>> kClearedLineWithGarbageBetweenAfter {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 01
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 02
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 03
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 04
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 05
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 06
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 07
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 08
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 09
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 11
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 12
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 13
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 14
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 13
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 14
  {1, 0, 1, 1, 1, 1, 1, 1, 0, 0}, // 15
  {1, 1, 0, 1, 1, 1, 1, 0, 0, 0}, // 17
  {1, 1, 0, 1, 1, 1, 1, 0, 1, 1}, // 18
  {1, 1, 0, 1, 1, 1, 1, 0, 1, 1}  // 20
};

TEST_CASE("ClearedLineWithGarbageBetween") {
  auto [asset_manager, matrix] = SetupTestHarness(kClearedLineWithGarbageBetweenBefore);

  auto tetrominos = asset_manager->GetTetrominos();

  Position insert_pos(kVisibleRowStart + 15, kVisibleColStart - 1);

  auto r = matrix->Commit(insert_pos, tetrominos.at(static_cast<int>(Tetromino::Type::I) - 1)->GetRotationData(Tetromino::Angle::A90));

  REQUIRE(r.size() == 2);
  REQUIRE(*matrix == kClearedLineWithGarbageBetweenAfter);
}

const std::vector<std::vector<int>> kTSpintMatrix {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 01
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 02
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 03
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 04
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 05
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 06
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 07
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 08
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 09
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 11
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 12
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 13
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 14
  {0, 0, 1, 1, 1, 1, 1, 1, 0, 0}, // 15
  {0, 0, 1, 1, 1, 1, 1, 1, 0, 0}, // 16
  {0, 0, 0, 1, 1, 1, 1, 0, 0, 0}, // 17
  {1, 1, 0, 1, 1, 1, 1, 0, 1, 1}, // 18
  {1, 0, 0, 1, 1, 1, 1, 0, 0, 1}, // 19
  {1, 1, 0, 1, 1, 1, 1, 0, 1, 1}  // 20
};

TEST_CASE("MatchT-spin") {
  REQUIRE(true);
}
