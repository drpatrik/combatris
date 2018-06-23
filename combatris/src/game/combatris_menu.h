#pragma once

#include "utility/menu_model.h"
#include "game/events.h"

class CombatrisMenu : public utility::MenuModel, public utility::MenuAction {
 public:
  const size_t kSelectCampaign = 1;
  const size_t kSelectLevel = 3;

  CombatrisMenu(Events& events) : MenuModel(), events_(events) {
    MenuModel::Add(MenuItemType::Name, "Campaign:");
    MenuModel::Add(MenuItemType::SubMenu,
                   {"Combatris", "Marathon", "Multiplayer VS.", "Multiplayer Marathon", "Multiplayer Battle"});
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
    if (kSelectCampaign == item) {
      events_.Push(Event::Type::MenuSetCampaign, sub_item + 1);
    } else if (kSelectLevel == item) {
      events_.Push(Event::Type::SetStartLevel, sub_item + 1);
    }
  }

 private:
  Events& events_;
};
