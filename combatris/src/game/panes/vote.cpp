#include "game/panes/vote.h"

#include <assert.h>

namespace {

const std::vector<CampaignType> kCampaignTypes = { CampaignType::Combatris, CampaignType::Marathon, CampaignType::Sprint, CampaignType::Ultra, CampaignType::Battle };

} // namespace

void Vote::Clear() {
  campaigns_.clear();
  for (auto type : kCampaignTypes) {
    campaigns_[type] = std::vector<PlayerInfo>();
  }
}

opt::optional<size_t> Vote::Cast(size_t host_id) {
  std::vector<std::pair<CampaignType, size_t>> vec;

  for (auto& campaign : campaigns_) {
    if (campaign.second.size() == 0) {
      continue;
    }
    if (campaign.second.size() == 1 && IsInList(host_id, campaign.second)) {
      return {};
    } else {
      vec.push_back(std::make_pair(campaign.first, campaign.second.size()));
    }
  }
  auto it = std::max_element(vec.begin(), vec.end(), [](const auto p1, const auto p2) { return p1.second < p2.second; });

  assert(it != vec.end());

  decltype(vec) matches;

  std::copy_if(it, vec.end(), std::back_inserter(matches), [&it](const auto& p) { return p.second == it->second; });

  if (matches.size() == 1 && !IsInList(host_id, campaigns_[it->first])) {
    return {};
  }
  std::sort(matches.begin(), matches.end(), [](const auto p1, const auto p2) { return p1.first > p2.first; });

  if (!IsInList(host_id, campaigns_[matches.begin()->first])) {
    return {};
  }

  return opt::make_optional<size_t>(campaigns_[matches.begin()->first].begin()->seed_);
}
