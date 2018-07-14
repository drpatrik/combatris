#pragma once

enum class CampaignRuleType { Normal, Marathon };

enum class CampaignType { None, Tetris, Marathon, MultiPlayerVS, MultiPlayerMarathon, MultiPlayerBattle };

inline int ToInt(CampaignType type) { return static_cast<int>(type); }

inline CampaignType ToCampaignType(int type) { return static_cast<CampaignType>(type); }

inline bool IsSinglePlayerCampaign(CampaignType type) { return CampaignType::Tetris == type || CampaignType::Marathon == type; }

inline bool IsMarathonCampaign(CampaignType type) { return CampaignType::Marathon == type || CampaignType::MultiPlayerMarathon == type; }

inline bool IsBattleCampaign(CampaignType type) { return CampaignType::MultiPlayerBattle == type; }

inline bool IsTetrisCampaign(CampaignType type) { return CampaignType::Tetris == type || CampaignType::MultiPlayerVS == type; }

inline CampaignRuleType CampaignToRuleType(CampaignType type) { return IsMarathonCampaign(type) ? CampaignRuleType::Marathon : CampaignRuleType::Normal; }
