#include "network/listener.h"

#include <iostream>
#include <functional>

namespace {

const int kWaitTime = 100;
const int kTimeOut = 5000;
const int64_t kConnectionCheckAliveInterval = 500;

} // namespace

namespace network {

int64_t Listener::VerifySequenceNumber(Listener::Connection& connection, const std::string& host_name, const PackageHeader& header) {
  const int64_t new_sequence_nr = header.sequence_nr();
  const int64_t old_sequence_nr = connection.sequence_nr_;
  const auto gap = new_sequence_nr - old_sequence_nr;

  if (gap < 0 || gap > 1) {
    std::cout << "Gap detected: " << host_name << ", got - " << new_sequence_nr << ", expected - "
              << old_sequence_nr + 1 << std::endl;
    return gap;
  }

  return 0;
}

void Listener::TerminateTimedOutConnections() {
  for (auto it = connections_.begin(); it != connections_.end();) {
    if (utility::time_in_ms() - it->second.timestamp_ >= kTimeOut) {
      std::cout << it->first << " timed out, connection terminated" << std::endl;
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
    auto size = server.Receive(&packages, sizeof(packages), kWaitTime);

    if (cancelled_.load(std::memory_order_acquire)) {
      break;
    }
    if ((utility::time_in_ms() - last_timeout_check) >= kConnectionCheckAliveInterval) {
      TerminateTimedOutConnections();
      last_timeout_check = utility::time_in_ms();
    }
    if (size == SOCKET_ERROR) {
      continue;
    }
    if (size < static_cast<ssize_t>(sizeof(packages))) {
      std::cout << "Incomplete package - " << size << std::endl;
      continue;
    }
    if (!packages.header_.VerifyHeader()) {
      std::cout << "Unknown package ignored" << std::endl;
      continue;
    }
    const auto& host_name = packages.header_.host_name();

    if (connections_.count(host_name) == 0) {
      connections_.insert(std::make_pair(host_name, Connection(packages.array_[0].header_.sequence_nr())));
    }
    auto& connection = connections_.at(host_name);

    auto index = VerifySequenceNumber(connection, host_name, packages.array_[0].header_);

    if (index < 0) {
      std::cout << "Old package(s) ignored" << std::endl;
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
          connection.SetJoined();
          break;
        case Request::Leave:
          process_request = false;
          if (connection.has_joined()) {
            connections_.erase(host_name);
            connection.SetLeft();
            process_request = true;
          }
          break;
        case Request::HeartBeat:
          process_request = false;
          break;
        default:
          break;
      }
      connection.Update(header);
      VerifySequenceNumber(connection, host_name, header);
      if (process_request) {
        queue_->Push(std::make_pair(host_name, package));
      }
    }
  }
}

} // namespace network
