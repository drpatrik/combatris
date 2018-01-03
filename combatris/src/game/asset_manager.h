#pragma once

#include "game/sprite.h"
#include "game/constants.h"
#include "tools/function_caller.h"

#include <string>
#include <vector>
#include <random>
#include <memory>

#include <SDL_ttf.h>

enum Font { Normal, Bold, Small, Large };

class AssetManager final {
 public:
  explicit AssetManager(SDL_Renderer *renderer);

  AssetManager(const AssetManager&) = delete;

  virtual ~AssetManager() noexcept {};

  virtual std::shared_ptr<const Sprite> GetSprite() const { return sprites_.at(distribution_(engine_)); }

  virtual std::shared_ptr<const Sprite> GetSprite(SpriteID id) { return sprites_.at(static_cast<int>(id)); }

  virtual TTF_Font *GetFont(int id) const { return fonts_[id].get(); }

 private:
  using UniqueFontPtr = std::unique_ptr<TTF_Font, function_caller<void(TTF_Font*), &TTF_CloseFont>>;
  using UniqueTexturePtr = std::unique_ptr<SDL_Texture, function_caller<void(SDL_Texture*), &SDL_DestroyTexture>>;

  std::vector<UniqueFontPtr> fonts_;
  std::vector<std::shared_ptr<Sprite>> sprites_;
  mutable std::mt19937 engine_ {std::random_device{}()};
  mutable std::uniform_int_distribution<int> distribution_{ 0, kNumSprites - 1 };
};
