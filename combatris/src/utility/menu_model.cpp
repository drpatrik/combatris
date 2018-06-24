#include "utility/menu_view.h"

#include <algorithm>
#include <stdexcept>

namespace utility {

size_t MenuModel::GetSelected() {
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

void MenuModel::Previous() {
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

void MenuModel::Next() {
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

void MenuModel::PrevSubItem() {
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

void MenuModel::NextSubItem() {
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

}  // namespace utility
