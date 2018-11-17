#pragma once

#include "utility/menu_model.h"
#include "game/events.h"

class CombatrisMenu : public utility::MenuModel, public utility::MenuAction {
 public:
  const size_t kSelectMode = 1;
  const size_t kSelectCampaign = 3;
  const size_t kSelectLevel = 5;
  const std::vector<std::string> kModes = { "Single Player", "Multi Player" };
  const std::vector<std::string> kSinglePlayerCampaigns = { "Combatris", "Marathon", "Sprint", "Ultra" };
  const std::vector<std::string> kMultiPlayerCampaigns =  {"Combatris", "Marathon", "Sprint", "Ultra", "Battle"};

  CombatrisMenu(Events& events) : MenuModel(), events_(events) {
    MenuModel::Add(MenuItemType::Name, "Mode:");
    MenuModel::Add(MenuItemType::SubMenu, kModes);
    MenuModel::Add(MenuItemType::Name, "Campaign:");
    MenuModel::Add(MenuItemType::SubMenu, kSinglePlayerCampaigns);
    MenuModel::Add(MenuItemType::Name, "Start Level:");

    std::vector<std::string> level_sub_menu;

    for (auto l = 0; l < kMaxNumberOfLevels; ++l) {
      level_sub_menu.push_back(std::to_string(l + 1));
    }
    MenuModel::Add(MenuItemType::SubMenu, level_sub_menu);
  }

  virtual ~CombatrisMenu() noexcept {}

  virtual void ItemSelected(size_t) override {}

  virtual void ItemSelected(size_t item, size_t sub_item) override {
    if (kSelectMode == item) {
      mode_ = static_cast<ModeType>(sub_item + 1);
      campaign_ = CampaignType::Combatris;
      Set(kSelectCampaign, (sub_item == 0) ? kSinglePlayerCampaigns : kMultiPlayerCampaigns);
      events_.Push(Event::Type::MenuSetModeAndCampaign, mode_, campaign_);
    } else if (kSelectCampaign == item) {
      campaign_ = static_cast<CampaignType>(sub_item + 1);
      events_.Push(Event::Type::MenuSetModeAndCampaign, mode_, campaign_);
    } else if (kSelectLevel == item) {
      events_.Push(Event::Type::SetStartLevel, static_cast<int>(sub_item + 1));
    }
  }

  virtual void ItemChanged(size_t) override {}

 private:
  ModeType mode_ = ModeType::SinglePlayer;
  CampaignType campaign_ = CampaignType::Combatris;
  Events& events_;
};
