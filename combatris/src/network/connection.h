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
        break;
      }
    }
    name_ = packages.header_.host_name();
    timestamp_ = utility::time_in_ms();
  }

  void Update(const Header& header) {
    if (sequence_nr_ != -1 && sequence_nr_ != header.sequence_nr()) {
        std::cout << "Error: Current: " << sequence_nr_ << " new: " << header.sequence_nr() << std::endl;
    }
    if (is_missing_) {
      std::cout << name_ << " is back" << "\n";
      is_missing_ = false;
    }
    sequence_nr_ = header.sequence_nr() + 1;
    timestamp_ = utility::time_in_ms();
  }

  int64_t VerifySequenceNumber(const Header& header) {
    if (sequence_nr_ == -1) {
      auto index = 0;
      std::swap(index, start_with_package_);
      return index;
    }
    const auto gap = static_cast<int64_t>(header.sequence_nr()) - sequence_nr_;

    if (gap != 0) {
      std::cout << name_ << ": gap detected, expected - " << sequence_nr_ << ", got " << header.sequence_nr() << "\n";
    }
    return gap;
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

  const std::string& name() const { return name_; }

 private:
  std::string name_;
  int start_with_package_ = 0;
  bool has_joined_ = false;
  mutable bool is_missing_ = false;
  int64_t timestamp_;
  int64_t sequence_nr_ = -1;
};

} // namespace Connection
