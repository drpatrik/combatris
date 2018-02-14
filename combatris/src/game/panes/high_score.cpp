#include "game/panes/high_score.h"

#include <fstream>

namespace {

const std::string kFilename("combatris.shs");

} // namespace


void HighScore::Read() {
   std::ifstream fs(kFilename);

  if (fs) {
    fs >> highscore_;
  }
}

void HighScore::Save() const {
  std::ofstream fs(kFilename);

  if (!fs) {
    std::cout << "Failed to save highscore to disk" << std::endl;
  } else {
    fs << highscore_ << std::endl;
  }
}
