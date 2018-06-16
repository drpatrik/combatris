#pragma once

enum class CampaignRuleType { Normal, Marathon };

enum class CampaignType { None, Tetris, Marathon, MultiPlayerVS, MultiPlayerMarathon, MultiPlayerBattle };

inline int ToInt(CampaignType type) { return static_cast<int>(type); }

inline CampaignType ToCampaignType(int type) { return static_cast<CampaignType>(type); }

inline bool IsSinglePlayerCampaign(CampaignType type) { return type == CampaignType::Tetris || type == CampaignType::Marathon ; }

inline bool IsMarathonCampaign(CampaignType type) { return type == CampaignType::Marathon || type == CampaignType::MultiPlayerMarathon; }

inline bool IsBattleCampaign(CampaignType type) { return type == CampaignType::MultiPlayerBattle; }

inline bool IsTetrisCampaign(CampaignType type) { return type == CampaignType::Tetris || type == CampaignType::MultiPlayerVS; }

inline CampaignRuleType CampaignToRuleType(CampaignType type) { return IsMarathonCampaign(type) ? CampaignRuleType::Marathon : CampaignRuleType::Normal; }
