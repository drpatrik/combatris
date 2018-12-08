#pragma once

#include "network/protocol.h"

using CampaignType = network::CampaignType;

enum class ModeType { None, SinglePlayer, MultiPlayer };

constexpr int ToInt(ModeType type) { return static_cast<int>(type); }

inline bool IsBattleCampaign(CampaignType type) { return CampaignType::Battle == type; }

inline bool IsSprintCampaign(CampaignType type) { return CampaignType::Sprint == type; }

inline bool IsUltraCampaign(CampaignType type) { return CampaignType::Ultra == type; }
