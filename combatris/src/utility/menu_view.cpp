#include "utility/menu_view.h"

namespace {

using namespace utility;

const Font kFontName = Font(Font::Typeface::ObelixPro, Font::Emphasis::Normal, 20);
const Font kFontItem = Font(Font::Typeface::ObelixPro, Font::Emphasis::Normal, 18);

}

namespace utility {

MenuView::MenuView(SDL_Renderer* renderer, const SDL_Rect& rc, const std::shared_ptr<Fonts>& fonts,
                   const std::shared_ptr<MenuModel>& menu_model, MenuAction* menu_action)
    : renderer_(renderer), rc_(rc), fonts_(fonts), menu_model_(menu_model), selected_item_(menu_model->GetSelected()), menu_action_(menu_action) {
  menu_model_->SetActionListener(this);
  const std::vector<std::string> kStrings = {"[", "]"};

  for (const auto& str : kStrings) {
    auto s = std::make_shared<Selection>();

    std::tie(s->texture_, s->rc_.w, s->rc_.h) =
        CreateTextureFromText(renderer_, fonts_->Get(kFontItem), str, Color::SteelGray);
    selection_.emplace_back(s);
  }
  for (int i = 0; i < static_cast<int>(menu_model_->size()); ++i) {
    items_.emplace_back(CreateItem(i));
  }
}

void MenuView::Render() {
  size_t pos = 0;
  int offset = rc_.y;

  for (auto& item : items_) {
    item->rc_.y = offset;

    if (pos == selected_item_) {
      auto& left = selection_.at(Left);
      left->rc_.x = item->rc_.x - (left->rc_.w + 10);
      left->rc_.y = offset;
      SDL_RenderCopy(renderer_, left->texture_.get(), nullptr, &left->rc_);
      auto& right = selection_.at(Right);
      right->rc_.x = item->rc_.x + (item->rc_.w + 10);
      right->rc_.y = offset;
      SDL_RenderCopy(renderer_, right->texture_.get(), nullptr, &right->rc_);
    }
    SDL_RenderCopy(renderer_, item->texture_.get(), nullptr, &item->rc_);
    offset += item->rc_.h + ((MenuModel::MenuItemType::Name == item->type_) ? 10 : 25);
    pos++;
  }
}

void MenuView::ItemSelected(size_t item) {
  selected_item_ = item;
  items_.at(item) = CreateItem(item);
  menu_action_->ItemSelected(item);
}

void MenuView::ItemSelected(size_t item, size_t sub_item) {
  selected_item_ = item;
  items_.at(item) = CreateItem(item);
  menu_action_->ItemSelected(item, sub_item);
}

void MenuView::ItemChanged(size_t item) { items_.at(item) = CreateItem(item); }

std::shared_ptr<MenuView::MenuItem> MenuView::CreateItem(size_t item_nr) {
  auto [type, text] = menu_model_->GetItem(item_nr);

  auto font = (MenuModel::MenuItemType::Name == type) ? kFontName : kFontItem;
  auto color = (MenuModel::MenuItemType::Name == type) ? Color::White : Color::Yellow;

  auto item = std::make_shared<MenuItem>(type);

  std::tie(item->texture_, item->rc_.w, item->rc_.h) =
      CreateTextureFromText(renderer_, fonts_->Get(font), text, color);

  item->rc_.x = rc_.x + Center(rc_.w, item->rc_.w);

  return item;
}

} // namespace utility
