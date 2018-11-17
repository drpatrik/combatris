#pragma once

enum class ModeType { None, SinglePlayer, MultiPlayer };

enum class CampaignType { None, Combatris, Marathon, Sprint, Ultra, Battle };

constexpr int ToInt(CampaignType type) { return static_cast<int>(type); }

constexpr int ToInt(ModeType type) { return static_cast<int>(type); }

inline bool IsBattleCampaign(CampaignType type) { return CampaignType::Battle == type; }

inline bool IsSprintCampaign(CampaignType type) { return CampaignType::Sprint == type; }

inline bool IsUltraCampaign(CampaignType type) { return CampaignType::Ultra == type; }
