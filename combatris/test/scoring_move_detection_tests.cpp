#include "test_utility.h"

#include "catch.hpp"

const std::vector<std::vector<int>> kPerfectClear {
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
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 19
  {1, 1, 1, 0, 0, 0, 0, 1, 1, 1}  // 20
};

TEST_CASE("DetectPerfectClear") {
  auto [assets, matrix] = SetupTestHarness(kPerfectClear);

  Position insert_pos((kMatrixFirstRow - 2) + 20, kMatrixFirstCol + 3);

  auto[lines_cleared, tspin_type, perfect_clear] =
      matrix->Commit(Tetromino::Type::I, Tetromino::Angle::A0, Tetromino::Move::Down, insert_pos);

  REQUIRE(lines_cleared.size() == 1);
  REQUIRE(TSpinType::None == tspin_type);
  REQUIRE(perfect_clear);
}

const std::vector<std::vector<int>> kTSpinMatrix {
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

TEST_CASE("DetectTSpin1") {
  auto [assets, matrix] = SetupTestHarness(kTSpinMatrix);
  Position insert_pos((kMatrixFirstRow - 2) + 18, kMatrixFirstCol + 6);

  auto [lines_cleared, tspin_type, perfect_clear] = matrix->Commit(Tetromino::Type::T, Tetromino::Angle::A90, Tetromino::Move::Rotation, insert_pos);

  REQUIRE(lines_cleared.size() == 0);
  REQUIRE(tspin_type == TSpinType::TSpin);
  REQUIRE_FALSE(perfect_clear);
}

const std::vector<std::vector<int>> kTSpinMatrix2 {
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
  {0, 0, 0, 1, 1, 0, 0, 0, 0, 0}, // 17
  {0, 0, 0, 1, 1, 0, 0, 0, 0, 0}, // 18
  {1, 1, 1, 1, 0, 0, 0, 1, 1, 1}, // 19
  {1, 1, 1, 1, 1, 0, 1, 1, 1, 1}  // 20
};

TEST_CASE("DetectTSpin2") {
  auto [assets, matrix] = SetupTestHarness(kTSpinMatrix2);
  Position insert_pos((kMatrixFirstRow - 2) + 18, kMatrixFirstCol + 4);

  auto [lines_cleared, tspin_type, perfect_clear] = matrix->Commit(Tetromino::Type::T, Tetromino::Angle::A270, Tetromino::Move::Rotation, insert_pos);

  REQUIRE(lines_cleared.size() == 1);
  REQUIRE(tspin_type == TSpinType::TSpin);
  REQUIRE_FALSE(perfect_clear);
}

const std::vector<std::vector<int>> kTSpinMatrix3 {
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
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 0}, // 17
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1}, // 18
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 19
  {8, 8, 8, 8, 8, 8, 8, 9, 8, 8}  // 20
};

TEST_CASE("DetectTSpin3") {
  auto [assets, matrix] = SetupTestHarness(kTSpinMatrix3);

  Position insert_pos((kMatrixFirstRow - 2) + 19, kMatrixFirstCol + 6);

  auto [lines_cleared, tspin_type, perfect_clear] = matrix->Commit(Tetromino::Type::T, Tetromino::Angle::A180, Tetromino::Move::Rotation, insert_pos);

  REQUIRE(lines_cleared.size() == 1);
  REQUIRE(tspin_type == TSpinType::TSpin);
  REQUIRE_FALSE(perfect_clear);
}

const std::vector<std::vector<int>> kTSpinMiniMatrix {
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
  {0, 0, 0, 0, 0, 0, 1, 0, 0, 0}, // 17
  {0, 0, 0, 0, 0, 0, 1, 0, 0, 0}, // 18
  {0, 0, 0, 0, 0, 0, 1, 0, 0, 0}, // 19
  {1, 1, 1, 1, 1, 0, 1, 1, 1, 1}  // 20
};

TEST_CASE("DetectTSpinMini") {
  auto [assets, matrix] = SetupTestHarness(kTSpinMiniMatrix);
  Position insert_pos((kMatrixFirstRow - 2) + 18, kMatrixFirstCol + 4);

  auto [lines_cleared, tspin_type, perfect_clear] = matrix->Commit(Tetromino::Type::T, Tetromino::Angle::A270, Tetromino::Move::Rotation, insert_pos);

  REQUIRE(lines_cleared.size() == 1);
  REQUIRE(tspin_type == TSpinType::TSpinMini);
  REQUIRE_FALSE(perfect_clear);
}
