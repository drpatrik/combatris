#pragma once

#include "utility/text.h"
#include "utility/fonts.h"
#include "utility/menu_model.h"

namespace utility {

class MenuView : protected MenuAction {
 public:
  MenuView(SDL_Renderer* renderer, const SDL_Rect& rc, const std::shared_ptr<Fonts>& fonts,
           const std::shared_ptr<MenuModel>& menu_model, MenuAction* menu_action);

  virtual ~MenuView() noexcept {}

  void SetY(int y) { rc_.y = y; }

  void Render();

 protected:
  virtual void ItemSelected(size_t item) override;

  virtual void ItemSelected(size_t item, size_t sub_item) override;

  virtual void ItemChanged(size_t item) override;

  struct MenuItem {
    MenuItem(MenuModel::MenuItemType type) : type_(type) {}
    MenuModel::MenuItemType type_;
    UniqueTexturePtr texture_;
    SDL_Rect rc_;
  };

  enum { Left, Right };

  struct Selection {
    UniqueTexturePtr texture_;
    SDL_Rect rc_;
  };

  std::shared_ptr<MenuItem> CreateItem(size_t item);

 private:
  SDL_Renderer* renderer_;
  SDL_Rect rc_;
  std::shared_ptr<Fonts> fonts_;
  std::vector<std::shared_ptr<Selection>> selection_;
  std::vector<std::shared_ptr<MenuItem>> items_;
  std::shared_ptr<MenuModel> menu_model_;
  size_t selected_item_;
  MenuAction* menu_action_ = nullptr;
};

} // namespace utility
