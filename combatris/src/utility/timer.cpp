#include "utility/timer.h"

#include <sstream>
#include <iomanip>

namespace utility {

std::string Timer::FormatTime(size_t seconds) const {
  std::stringstream ss;

  int minutes = static_cast<int>(seconds / 60.0);

  ss << std::setfill('0') << std::setw(2) << minutes << ":" << std::setfill('0') << std::setw(2) << seconds % 60;

  return ss.str();
}

} // namespace utility
