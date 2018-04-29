#pragma once

#include "utility/timer.h"
#include "network/protocol.h"
#include "network/protocol_timing_settings.h"

#include <iostream>

namespace network {

class Connection final {
 public:
  Connection(const Packages& packages) {
    for (int index = 0; index < packages.size(); ++index) {
      if (packages.array_[index].header_.request() == Request::Join) {
        start_with_package_ = index;
        sequence_nr_ = packages.array_[index].header_.sequence_nr();
        break;
      }
    }
    name_ = packages.header_.host_name();
    timestamp_ = utility::time_in_ms();
  }

  void Update(const Header& header) {
    sequence_nr_ = header.sequence_nr();
    timestamp_ = utility::time_in_ms();
    if (is_missing_) {
      std::cout << name_ << " is back" << "\n";
      is_missing_ = false;
    }
  }

int64_t VerifySequenceNumber(const Header& header) {
  if (start_with_package_ != 0) {
    return start_with_package_;
  }
  const int64_t current_sequence_nr = header.sequence_nr();
  const int64_t prev_sequence_nr = sequence_nr_;

  if (current_sequence_nr == 0 && prev_sequence_nr == 0) {
    return 0;
  }
  const auto gap = current_sequence_nr - prev_sequence_nr;

  if (gap < 0 || gap > 1) {
    std::cout << name_ << ": gap detected, expected - " << prev_sequence_nr + 1 << ", got " << current_sequence_nr << "\n";
  }

  return (gap == 1) ? 0 : gap;
}

  bool has_timed_out() const {
    auto time_since_last_update = utility::time_in_ms() - timestamp_;

    if (time_since_last_update >= kConnectionMissing) {
      std::cout << name_ << " is missing, last update " << time_since_last_update << " ms ago\n";
      is_missing_ = true;
    }

    return time_since_last_update >= kConnectionTimeOut;
  }

  bool has_joined() const { return has_joined_; }

  void SetHasJoined() { has_joined_ = true; }

  void SetHasLeft() { has_joined_ = false; }

  const std::string& name() const { return name_; }

 private:
  bool has_joined_ = false;
  mutable bool is_missing_ = false;
  int start_with_package_ = 0;
  std::string name_;
  uint32_t sequence_nr_;
  int64_t timestamp_;
};

} // namespace Connection
