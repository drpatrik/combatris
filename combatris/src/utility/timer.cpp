#include "utility/timer.h"

#include <sstream>
#include <iomanip>

namespace utility {

std::string FormatTimeMMSSHS(size_t t) {
  std::stringstream ss;

  const size_t minutes = static_cast<size_t>(t / 60000.0);
  const size_t seconds = static_cast<size_t>((t % 60000) / 1000.0);
  const size_t hs =  (t % 1000) / 10;

  ss << std::setfill('0') << std::setw(2) << minutes << ":" << std::setfill('0') << std::setw(2) << seconds << ":" << std::setw(2) << hs;

  return ss.str();

}

std::string Timer::FormatTime(size_t seconds) const {
  std::stringstream ss;

  const int minutes = static_cast<int>(seconds / 60.0);

  ss << std::setfill('0') << std::setw(2) << minutes << ":" << std::setfill('0') << std::setw(2) << seconds % 60;

  return ss.str();
}

std::string Clock::FormatTime(size_t t) const { return FormatTimeMMSSHS(t); }

} // namespace utility
