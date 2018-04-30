#include "test_utility.h"

#include "catch.hpp"

const std::vector<std::vector<int>> kSendLinesBefore {
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
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 15
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 16
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 17
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 18
  {0, 0, 0, 0, 0, 0, 1, 1, 1, 0}, // 19
  {1, 1, 1, 1, 1, 1, 1, 0, 1, 1}  // 20
};

TEST_CASE("SendLines") {
  auto matrix = SetupTestHarnessMatrixOnly(kSendLinesBefore);

  matrix->InsertLines(20);
  matrix->RemoveLines();
  REQUIRE(*matrix == kSendLinesBefore);
  matrix->InsertLines(3);
  matrix->RemoveLines();
  REQUIRE(*matrix == kSendLinesBefore);
  matrix->InsertLines(1);
  matrix->InsertLines(1);
  matrix->InsertLines(18);
  matrix->InsertLines(1);
  matrix->RemoveLines();
  REQUIRE(*matrix == kSendLinesBefore);
}

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
  auto [assets, matrix] = SetupTestHarness(kClearTopRowBefore);

  auto tetrominos = assets->GetTetrominos();

  Position insert_pos(0, kVisibleColStart);

  auto [lines_cleared, tspin_type, perfect_clear] = matrix->Commit(Tetromino::Type::I, Tetromino::Move::Down, insert_pos,
                 tetrominos.at(static_cast<int>(Tetromino::Type::I) - 1)
                     ->GetRotationData(Tetromino::Angle::A0));

  REQUIRE(lines_cleared.size() == 1);
  REQUIRE(*matrix == kClearTopRowAfter);
  REQUIRE(TSpinType::None == tspin_type);
  REQUIRE_FALSE(perfect_clear);
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
  auto [assets, matrix] = SetupTestHarness(kClearedLineWithGarbageBetweenBefore);

  auto tetrominos = assets->GetTetrominos();

  Position insert_pos(kVisibleRowStart + 15, kVisibleColStart - 1);

  auto[lines_cleared, tspin_type, perfect_clear] =
      matrix->Commit(Tetromino::Type::I, Tetromino::Move::Down, insert_pos,
                     tetrominos.at(static_cast<int>(Tetromino::Type::I) - 1)
                         ->GetRotationData(Tetromino::Angle::A90));

  REQUIRE(lines_cleared.size() == 2);
  REQUIRE(*matrix == kClearedLineWithGarbageBetweenAfter);
  REQUIRE(TSpinType::None == tspin_type);
  REQUIRE_FALSE(perfect_clear);
}
