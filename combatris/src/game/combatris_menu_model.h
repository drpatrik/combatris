#pragma once

#include "utility/menu_model.h"
#include "game/constants.h"

class CombatrisMenuModel : public utility::MenuModel {
 public:
  CombatrisMenuModel() : MenuModel() {
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
};

const int kMenuHeight = 280;
