#include "network/listener.h"

namespace network {

int64_t Listener::VerifySequenceNumber(Listener::Connection& connection, const std::string& name, const PackageHeader& header) {
  const int64_t current_sequence_nr = header.sequence_nr();
  const int64_t prev_sequence_nr = connection.sequence_nr_;

  if (current_sequence_nr == 0 && prev_sequence_nr == 0) {
    return 0;
  }
  const auto gap = current_sequence_nr - prev_sequence_nr;

  if (gap < 0 || gap > 1) {
    std::cout << name << ": gap detected, expected - " << prev_sequence_nr + 1 << ", got " << current_sequence_nr << "\n";
  }

  return (gap == 1) ? 0 : gap;
}

void Listener::TerminateTimedOutConnections() {
  for (auto it = connections_.begin(); it != connections_.end();) {
    if (it->second.has_timed_out(it->first)) {
      std::cout << it->first << " timed out, connection terminated" << "\n";
      queue_->Push(std::make_pair(it->first, CreatePackage(Request::Leave)));
      it = connections_.erase(it);
    } else {
      ++it;
    }
  }
}

void Listener::Run() {
  UDPServer server(GetPort());
  Packages packages;
  auto last_timeout_check  = utility::time_in_ms();

  for (;;) {
    if (cancelled_.load(std::memory_order_acquire)) {
      break;
    }
    auto size = server.Receive(&packages, sizeof(packages), kWaitForIncomingPackages);

    if (cancelled_.load(std::memory_order_acquire)) {
      break;
    }
    if (size == SOCKET_ERROR) {
      exit(0);
    }
    if ((utility::time_in_ms() - last_timeout_check) >= kConnectionCheckAliveInterval) {
      TerminateTimedOutConnections();
      last_timeout_check = utility::time_in_ms();
    }
    if (size == SOCKET_TIMEOUT) {
      continue;
    }
    if (size < static_cast<ssize_t>(sizeof(packages))) {
      std::cout << "incomplete package - " << size << std::endl;
      continue;
    }
    if (!packages.header_.VerifyHeader()) {
      std::cout << "unknown package ignored" << std::endl;
      continue;
    }
    int start_index = 0;
    const auto& host_name = packages.header_.host_name();

    if (connections_.count(host_name) == 0) {
      auto [it, inserted] = connections_.insert(std::make_pair(host_name, Connection(packages)));
      start_index = it->second.start_index_;
    }
    auto& connection = connections_.at(host_name);

    auto index = (start_index == 0 ) ? VerifySequenceNumber(connection, host_name, packages.array_[0].header_) : start_index;

    if (index < 0) {
      std::cout << "old package(s) ignored" << std::endl;
      continue;
    }
    if (index > packages.size() || index >= kWindowSize) {
      std::cout << host_name << " has lost too many packages, connection will be terminated" << std::endl;
      connections_.erase(host_name);
      queue_->Push(std::make_pair(host_name, CreatePackage(Request::Leave)));
      continue;
    }
    std::vector<Package> package_vector;

    for (auto i = index; i >= 0; --i) {
      package_vector.push_back(packages.array_[i]);
    }
    for (const auto& package : package_vector) {
      bool process_request = true;
      const auto& header = package.header_;

      if (!header.VerifyHeader()) {
        std::cout << "Unknown package ignored" << std::endl;
        continue;
      }
      switch (header.request()) {
        case Request::Join:
          process_request = !connection.has_joined();
          connection.SetHasJoined();
          break;
        case Request::Leave:
          process_request = false;
          if (connection.has_joined()) {
            connections_.erase(host_name);
            connection.SetHasLeft();
            process_request = true;
          }
          break;
        case Request::HeartBeat:
          process_request = false;
          break;
        default:
          break;
      }
      connection.Update(host_name, header);
      if (process_request) {
        queue_->Push(std::make_pair(host_name, package));
      }
    }
  }
}

} // namespace network
