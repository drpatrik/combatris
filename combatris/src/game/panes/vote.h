#pragma once

#include "game/events.h"

#if __has_include(<optional>)
#include <optional>
namespace opt = std;
#else
#include <experimental/optional>
namespace opt = std::experimental;
#endif

#include <map>

class Vote {
 public:
  Vote() { Clear(); }

  void Add(size_t host_id,CampaignType type, size_t seed) {
    auto& v = campaigns_.at(type);

    v.push_back(PlayerInfo(host_id, seed));
    std::sort(v.begin(), v.end());
  }

  void Clear();

  opt::optional<size_t> Cast(size_t host_id) const;

 private:
  struct PlayerInfo {
    PlayerInfo(size_t host_id, size_t seed) : host_id_(host_id), seed_(seed) {}

    inline bool operator < (const PlayerInfo& pi) const { return pi.host_id_ < host_id_; }

    inline bool operator == (size_t host_id) const { return host_id == host_id_; }

    size_t host_id_;
    size_t seed_;
  };

  inline bool IsInList(size_t host_id, const std::vector<PlayerInfo>& v) const { return v.end() != std::find(v.begin(), v.end(), host_id); }

  std::map<CampaignType, std::vector<PlayerInfo>> campaigns_;
};
