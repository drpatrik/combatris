#include "test_utility.h"

#include "catch.hpp"

const std::vector<std::vector<int>> kPerfectClear {
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
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 20
  {1, 1, 1, 0, 0, 0, 0, 1, 1, 1}  // 21
};

TEST_CASE("DetectPerfectClear") {
  auto [assets, matrix] = SetupTestHarness(kPerfectClear);

  auto tetrominos = assets->GetTetrominos();

  Position insert_pos(20, kVisibleColStart + 3);

  auto[lines_cleared, tspin_type, perfect_clear] =
      matrix->Commit(Tetromino::Type::I, Tetromino::Move::Down, insert_pos,
                     tetrominos.at(static_cast<int>(Tetromino::Type::I) - 1)
                         ->GetRotationData(Tetromino::Angle::A0));

  REQUIRE(lines_cleared.size() == 1);
  REQUIRE(TSpinType::None == tspin_type);
  REQUIRE(perfect_clear);
};
