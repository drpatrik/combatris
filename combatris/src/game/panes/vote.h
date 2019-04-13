#pragma once

#include "game/events.h"

#include <optional>
#include <map>

class Vote {
 public:
  Vote() { Clear(); }

  void Add(uint64_t host_id,CampaignType type, uint64_t seed) {
    auto& v = campaigns_.at(type);

    v.push_back(PlayerInfo(host_id, seed));
    std::sort(v.begin(), v.end());
  }

  void Clear();

  std::optional<uint64_t> Cast(uint64_t host_id) const;

 private:
  struct PlayerInfo {
    PlayerInfo(uint64_t host_id, uint64_t seed) : host_id_(host_id), seed_(seed) {}

    inline bool operator < (const PlayerInfo& pi) const { return pi.host_id_ < host_id_; }

    inline bool operator == (uint64_t host_id) const { return host_id == host_id_; }

    uint64_t host_id_;
    uint64_t seed_;
  };

  inline bool IsInList(uint64_t host_id, const std::vector<PlayerInfo>& v) const { return v.end() != std::find(v.begin(), v.end(), host_id); }

  std::map<CampaignType, std::vector<PlayerInfo>> campaigns_;
};
