#pragma once


#include <string>
#include <vector>
#include <tuple>
#include <iostream>

namespace utility {

class MenuAction {
 public:
  virtual ~MenuAction() {}

  virtual void ItemSelected(size_t item) = 0;

  virtual void ItemSelected(size_t item, size_t sub_item) = 0;

  virtual void ItemChanged(size_t item) = 0;
};

class MenuModel {
 public:
  enum class MenuItemType { SubMenu, Name };

  MenuModel() {}

  inline void SetActionListener(MenuAction* menu_action) { menu_action_ = menu_action; }

  size_t GetSelected();

  inline size_t GetSelected(size_t item) const { return std::get<1>(model_.at(item)); }

  inline bool HasSubItems(size_t item) const { return std::get<2>(model_.at(item)).size() > 1; }

  inline bool IsSelected(size_t item) const { return selected_item_ == item; }

  inline size_t size() const { return model_.size(); }

  void Previous();

  void Next();

  void PrevSubItem();

  void NextSubItem();

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

  void Set(size_t item, const std::vector<std::string>& items) {
    auto& e = model_.at(item);

    std::get<1>(e) = 0;
    std::get<2>(e) = items;

    if (nullptr != menu_action_) {
      menu_action_->ItemChanged(item);
    }
  }

 private:
  std::vector<std::tuple<MenuItemType, size_t, std::vector<std::string>>> model_;
  size_t selected_item_ = 0;
  MenuAction* menu_action_ = nullptr;
  bool set_selected_item_ = true;
};

}  // namespace utility
