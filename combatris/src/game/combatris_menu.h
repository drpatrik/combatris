#pragma once

#include "game/events.h"
#include "utility/menu_model.h"
#include "utility/game_controller.h"

class CombatrisMenu : public utility::MenuModel, public utility::MenuAction, public utility::GameController::Callback {
 public:
  const size_t kSelectMode = 1;
  const size_t kSelectInput = 3;
  const size_t kSelectCampaign = 5;
  const size_t kSelectLevel = 7;

  const std::vector<std::string> kModes = { "Single Player", "Multi Player" };
  const std::vector<std::string> kDefaultInput =  { "Keyboard" };
  const std::vector<std::string> kSinglePlayerCampaigns = { "Combatris", "Marathon", "Sprint", "Ultra", "Royal" };
  const std::vector<std::string> kMultiPlayerCampaigns =  {"Combatris", "Marathon", "Sprint", "Ultra", "Royal", "Battle"};

  CombatrisMenu(Events& events, std::shared_ptr<utility::GameController> game_controller) :
      MenuModel(), events_(events), game_controller_(game_controller) {
    MenuModel::Add(MenuItemType::Name, "Mode:");
    MenuModel::Add(MenuItemType::SubMenu, kModes);
    MenuModel::Add(MenuItemType::Name, "Input:");
    MenuModel::Add(MenuItemType::SubMenu, kDefaultInput);
    MenuModel::Add(MenuItemType::Name, "Campaign:");
    MenuModel::Add(MenuItemType::SubMenu, kSinglePlayerCampaigns);
    MenuModel::Add(MenuItemType::Name, "Start Level:");

    std::vector<std::string> level_sub_menu;

    for (auto l = 0; l < kMaxNumberOfLevels; ++l) {
      level_sub_menu.push_back(std::to_string(l + 1));
    }
    MenuModel::Add(MenuItemType::SubMenu, level_sub_menu);
    game_controller_->AddCallback(this);
  }

  virtual ~CombatrisMenu() noexcept {}

 protected:
  virtual void ItemSelected(size_t) override {}

  virtual void ItemSelected(size_t item, size_t sub_item) override {
    if (kSelectMode == item) {
      mode_ = static_cast<ModeType>(sub_item + 1);
      campaign_ = CampaignType::Combatris;
      Set(kSelectCampaign, (sub_item == 0) ? kSinglePlayerCampaigns : kMultiPlayerCampaigns);
      events_.Push(Event::Type::MenuSetModeAndCampaign, mode_, campaign_);
    } else if (kSelectInput == item) {
      game_controller_->Detach(current_game_controller_);
      current_game_controller_ = map_item_to_controller.at(sub_item);
      game_controller_->Attach(current_game_controller_);
    } else if (kSelectCampaign == item) {
      campaign_ = static_cast<CampaignType>(sub_item + 1);
      events_.Push(Event::Type::MenuSetModeAndCampaign, mode_, campaign_);
    } else if (kSelectLevel == item) {
      events_.Push(Event::Type::SetStartLevel, static_cast<int>(sub_item + 1));
    }
  }

  virtual void ItemChanged(size_t) override {}

  virtual void AddGameController(int, const std::string&) override { UpdateInputItems(); }

  virtual void RemoveGameController(int) override { UpdateInputItems(); }

  void UpdateInputItems() {
    map_item_to_controller.clear();
    map_item_to_controller.push_back(-1);
    auto game_controllers = game_controller_->GetGameControllers();

    std::transform(game_controllers.begin(), game_controllers.end(), std::back_inserter(map_item_to_controller),
                   [](const auto& p) { return p.first; } );

    auto items = game_controller_->GetGameControllersAsList();

    items.insert(items.begin(), kDefaultInput[0]);
    Set(kSelectInput, items);
  }

 private:
  ModeType mode_ = ModeType::SinglePlayer;
  CampaignType campaign_ = CampaignType::Combatris;
  Events& events_;
  int current_game_controller_ = -1; // Keyboard
  std::vector<int> map_item_to_controller;
  std::shared_ptr<utility::GameController> game_controller_;
};
