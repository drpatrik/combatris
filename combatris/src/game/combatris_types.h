#pragma once

#include "network/protocol.h"

using CampaignType = network::CampaignType;

enum class ModeType { None, SinglePlayer, MultiPlayer };

constexpr int ToInt(ModeType type) { return static_cast<int>(type); }

inline bool IsBattleCampaign(CampaignType type) { return CampaignType::Battle == type; }

inline bool IsSprintCampaign(CampaignType type) { return CampaignType::Sprint == type; }

inline bool IsUltraCampaign(CampaignType type) { return CampaignType::Ultra == type; }

inline bool IsRoyalCampaign(CampaignType type) { return CampaignType::Royal == type; }

inline bool IsMarathonCampaign(CampaignType type) { return CampaignType::Marathon == type; }

inline bool IsIn(CampaignType type, const std::initializer_list<CampaignType>& list) {
  return std::find(list.begin(), list.end(), type) != list.end();
}
