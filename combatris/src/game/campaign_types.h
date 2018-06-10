#pragma once

enum class CampaignType { None, Tetris, Marathon, MultiPlayerVS, MultiPlayerMarathon, MultiPlayerBattle };

inline int ToInt(CampaignType type) { return static_cast<int>(type); }

inline CampaignType ToCampaignType(int type) { return static_cast<CampaignType>(type); }
