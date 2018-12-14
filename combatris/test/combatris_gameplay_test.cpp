#include "test_utility.h"
#include "game/tetromino_sprite.h"

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

  REQUIRE(!matrix->HasSolidLines());

  matrix->InsertSolidLines(20);
  REQUIRE(matrix->HasSolidLines());
  matrix->RemoveSolidLines();
  REQUIRE(!matrix->HasSolidLines());
  REQUIRE(*matrix == kSendLinesBefore);
  matrix->InsertSolidLines(3);
  matrix->RemoveSolidLines();
  REQUIRE(*matrix == kSendLinesBefore);
  matrix->InsertSolidLines(1);
  matrix->InsertSolidLines(1);
  matrix->InsertSolidLines(18);
  matrix->InsertSolidLines(1);
  matrix->RemoveSolidLines();
  REQUIRE(*matrix == kSendLinesBefore);
}

const std::vector<std::vector<int>> kTopOutGameover {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 01
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 02
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 03
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 04
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 05
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 06
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 07
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 08
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 09
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 11
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 12
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 13
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 14
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 15
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 16
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 17
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 18
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 19
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}  // 20
};

TEST_CASE("SendLinesTopOutGameover") {
  auto matrix = SetupTestHarnessMatrixOnly(kTopOutGameover);

  REQUIRE(!matrix->HasSolidLines());
  REQUIRE(matrix->InsertSolidLines(20));
  REQUIRE(!matrix->InsertSolidLines(2));
  matrix->RemoveSolidLines();
  REQUIRE(!matrix->HasSolidLines());
  REQUIRE(*matrix == kTopOutGameover);
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

  Position insert_pos(0, kMatrixFirstCol);

  auto [lines_cleared, tspin_type, perfect_clear] =
      matrix->Commit(Tetromino::Type::I, Tetromino::Angle::A0, Tetromino::Move::Down, insert_pos);

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

  Position insert_pos(kMatrixFirstRow + 15, kMatrixFirstCol - 1);

  auto[lines_cleared, tspin_type, perfect_clear] =
      matrix->Commit(Tetromino::Type::I, Tetromino::Angle::A90, Tetromino::Move::Down, insert_pos);

  REQUIRE(lines_cleared.size() == 2);
  REQUIRE(*matrix == kClearedLineWithGarbageBetweenAfter);
  REQUIRE(TSpinType::None == tspin_type);
  REQUIRE_FALSE(perfect_clear);
}

const std::vector<std::vector<int>> kAboveSkyline {
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

TEST_CASE("IsAboveSkylineTest") {
  auto [assets, matrix] = SetupTestHarness(kAboveSkyline);

  auto tetrominos = assets->GetTetrominos();
  auto rotation_data = tetrominos[static_cast<int>(Tetromino::Type::J) - 1]->GetRotationData(Tetromino::Angle::A0);

  REQUIRE(matrix->IsAboveSkyline(kSpawnPosition, rotation_data));

  rotation_data = tetrominos[static_cast<int>(Tetromino::Type::J) - 1]->GetRotationData(Tetromino::Angle::A180);

  REQUIRE(!matrix->IsAboveSkyline(kSpawnPosition, rotation_data));
}
