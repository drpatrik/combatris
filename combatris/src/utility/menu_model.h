#pragma once

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

namespace utility {

class MenuAction {
 public:
  virtual void ItemSelected(size_t item) = 0;

  virtual void ItemSelected(size_t item, size_t sub_item) = 0;
};

class MenuModel {
 public:
  enum class MenuItemType { SubMenu, Name };

  MenuModel() {}

  void SetActionListener(MenuAction* menu_action) { menu_action_ = menu_action; }

  void Select(size_t item) {
    if (item >= model_.size()) {
      throw std::out_of_range("item not valid" + std::to_string(item));
    }

    if (item != selected_item_) {
      selected_item_ = item;
      menu_action_->ItemSelected(item);
    }
  }

  void Select(size_t item, size_t sub_item) {
    if (item >= model_.size()) {
      throw std::out_of_range("item not valid" + std::to_string(item));
    }
    auto& [type, selected, sub_items] = model_.at(item);

    if (type != MenuItemType::SubMenu) {
      throw std::invalid_argument("item has not a submenu");
    }
    if (sub_item >= sub_items.size()) {
      throw std::out_of_range("sub_item not valid" + std::to_string(sub_item));
    }
    if (selected != sub_item) {
      selected = sub_item;
      menu_action_->ItemSelected(item, sub_item);
    }
  }

  size_t GetSelected() {
    if (set_selected_item_) {
      for (size_t i = 0; i < model_.size(); ++i) {
        if (MenuItemType::SubMenu == std::get<0>(model_.at(i))) {
          selected_item_ = i;
          break;
        }
      }
      set_selected_item_ = false;
    }
    return selected_item_;
  }

  size_t GetSelected(size_t item) const { return std::get<1>(model_.at(item)); }

  void Previous() {
    bool accept = false;
    auto new_selection = selected_item_;

    if (0 == new_selection) {
      return;
    }
    while (true) {
      new_selection--;
      if (MenuItemType::SubMenu == std::get<0>(model_.at(new_selection))) {
        accept = true;
        break;
      }
      if (0 == new_selection) {
        break;
      }
    }
    if (accept && new_selection != selected_item_) {
      selected_item_ = new_selection;
      menu_action_->ItemSelected(selected_item_);
    }
  }

  void Next() {
    bool accept = false;
    auto new_selection = selected_item_;

    if (model_.size() - 1 == new_selection) {
      return;
    }
    while (true) {
      new_selection++;
      if (MenuItemType::SubMenu == std::get<0>(model_.at(new_selection))) {
        accept = true;
        break;
      }
      if (new_selection >= model_.size()) {
        break;
      }
    }
    if (accept && new_selection != selected_item_) {
      selected_item_ = new_selection;
      menu_action_->ItemSelected(selected_item_);
    }
  }

  void PrevSubItem() {
    auto type = std::get<0>(model_.at(selected_item_));

    if (MenuItemType::SubMenu != type) {
      return;
    }
    auto& selected = std::get<1>(model_.at(selected_item_));

    if (0 == selected) {
      return;
    }
    auto new_selection = std::max(selected - 1, static_cast<size_t>(0));

    if (new_selection != selected) {
      selected = new_selection;
      menu_action_->ItemSelected(selected_item_, selected);
    }
  }

  void NextSubItem() {
    auto& [type, selected, sub_items] = model_.at(selected_item_);

    if (MenuItemType::SubMenu != type) {
      return;
    }
    auto new_selection = std::min(selected + 1, sub_items.size() - 1);

    if (new_selection != selected) {
      selected = new_selection;
      menu_action_->ItemSelected(selected_item_, selected);
    }
  }

  bool HasSubItems(size_t item) const { return std::get<2>(model_.at(item)).size() > 1; }

  bool IsSelected(size_t item) const { return selected_item_ == item; }

  size_t size() const { return model_.size(); }

  std::pair<MenuItemType, std::string> GetItem(size_t item) const {
    const auto& [type, selected, sub_items] = model_.at(item);

    return std::make_pair(type, sub_items.at(selected));
  }

 protected:
  void Add(MenuItemType type, const std::vector<std::string>& items) {
    model_.emplace_back(std::make_tuple(type, 0, items));
    set_selected_item_ = true;
  }

  void Add(MenuItemType type, const std::string& item) {
    model_.emplace_back(std::make_tuple(type, 0, std::vector<std::string>{item}));
    set_selected_item_ = true;
  }

 private:
  std::vector<std::tuple<MenuItemType, size_t, std::vector<std::string>>> model_;
  size_t selected_item_ = 0;
  MenuAction* menu_action_ = nullptr;
  bool set_selected_item_ = true;
};

}  // namespace utility
