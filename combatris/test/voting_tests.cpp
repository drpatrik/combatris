#include "game/panes/vote.h"

#include "catch.hpp"

TEST_CASE("TestVoteOnlyOne") {
  Vote vote;

  vote.Add(2, CampaignType::Combatris, 2);
  vote.Add(1, CampaignType::Combatris, 1);

  REQUIRE(vote.Cast(1) == 2);
}

TEST_CASE("TestVoteTwoOfEach") {
  Vote vote;

  vote.Add(1, CampaignType::Battle, 1);
  vote.Add(2, CampaignType::Battle, 2);

  vote.Add(3, CampaignType::Ultra, 3);
  vote.Add(4, CampaignType::Ultra, 4);

  REQUIRE(vote.Cast(1) == 2);
  REQUIRE_FALSE(vote.Cast(3));
}

TEST_CASE("TestVoteThreeOfEach") {
  Vote vote;

  vote.Add(1, CampaignType::Sprint, 1);
  vote.Add(2, CampaignType::Sprint, 2);

  vote.Add(3, CampaignType::Battle, 3);
  vote.Add(4, CampaignType::Battle, 4);

  vote.Add(5, CampaignType::Ultra, 5);
  vote.Add(6, CampaignType::Ultra, 6);

  REQUIRE_FALSE(vote.Cast(1));
  REQUIRE_FALSE(vote.Cast(2));
  REQUIRE(vote.Cast(3) == 4);
  REQUIRE(vote.Cast(4) == 4);
  REQUIRE_FALSE(vote.Cast(5));
  REQUIRE_FALSE(vote.Cast(6));
}

TEST_CASE("TestVoteTwoTwoAndOne") {
  Vote vote;

  vote.Add(1, CampaignType::Sprint, 1);
  vote.Add(2, CampaignType::Sprint, 2);

  vote.Add(4, CampaignType::Battle, 4);

  vote.Add(5, CampaignType::Ultra, 5);
  vote.Add(6, CampaignType::Ultra, 6);

  REQUIRE_FALSE(vote.Cast(1));
  REQUIRE_FALSE(vote.Cast(2));
  REQUIRE_FALSE(vote.Cast(4));
  REQUIRE(vote.Cast(5) == 6);
  REQUIRE(vote.Cast(6) == 6);
}

TEST_CASE("TestVoteOneForEveryCampaignType") {
  Vote vote;

  vote.Add(1, CampaignType::Combatris, 1);
  vote.Add(2, CampaignType::Marathon, 2);

  vote.Add(3, CampaignType::Ultra, 3);
  vote.Add(4, CampaignType::Battle, 4);

  REQUIRE_FALSE(vote.Cast(1));
  REQUIRE_FALSE(vote.Cast(2));
  REQUIRE_FALSE(vote.Cast(3));
  REQUIRE_FALSE(vote.Cast(4));
}

TEST_CASE("TestVoteNone") {
  Vote vote;

  REQUIRE_THROWS_AS(vote.Cast(1), std::out_of_range);
}
