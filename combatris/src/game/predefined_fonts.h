#pragma once

#include "utility/fonts.h"

#include <vector>

const utility::Font Normal200 = utility::Font(utility::Font::Typeface::Cabin, utility::Font::Emphasis::Normal, 200);
const utility::Font Normal55 = utility::Font(utility::Font::Typeface::Cabin, utility::Font::Emphasis::Normal, 55);
const utility::Font Bold55 = utility::Font(utility::Font::Typeface::Cabin, utility::Font::Emphasis::Bold, 55);
const utility::Font Normal45 = utility::Font(utility::Font::Typeface::Cabin, utility::Font::Emphasis::Normal, 45);
const utility::Font Bold45 = utility::Font(utility::Font::Typeface::Cabin, utility::Font::Emphasis::Bold, 45);
const utility::Font Normal35 = utility::Font(utility::Font::Typeface::Cabin, utility::Font::Emphasis::Normal, 35);
const utility::Font Bold35 = utility::Font(utility::Font::Typeface::Cabin, utility::Font::Emphasis::Bold, 35);
const utility::Font Bold30 = utility::Font(utility::Font::Typeface::Cabin, utility::Font::Emphasis::Normal, 30);
const utility::Font Normal25 = utility::Font(utility::Font::Typeface::Cabin, utility::Font::Emphasis::Normal, 25);
const utility::Font Bold25 = utility::Font(utility::Font::Typeface::Cabin, utility::Font::Emphasis::Bold, 25);
const utility::Font Normal15 = utility::Font(utility::Font::Typeface::Cabin, utility::Font::Emphasis::Normal, 15);
const utility::Font ObelixPro18 = utility::Font(utility::Font::Typeface::ObelixPro, utility::Font::Emphasis::Normal, 18);
const utility::Font ObelixPro35 = utility::Font(utility::Font::Typeface::ObelixPro, utility::Font::Emphasis::Normal, 35);
const utility::Font ObelixPro40 = utility::Font(utility::Font::Typeface::ObelixPro, utility::Font::Emphasis::Normal, 40);
const utility::Font ObelixPro50 = utility::Font(utility::Font::Typeface::ObelixPro, utility::Font::Emphasis::Normal, 50);

const std::vector<utility::Font> kFontsToPreload = {
  Normal200,
  Normal55,
  Bold55,
  Normal45,
  Bold45,
  Normal35,
  Bold35,
  Bold30,
  Normal25,
  Bold25,
  Normal15,
  ObelixPro18,
  ObelixPro35,
  ObelixPro40,
  ObelixPro50
};
