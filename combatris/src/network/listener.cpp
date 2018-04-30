#include "network/listener.h"

namespace network {

void Listener::TerminateTimedOutConnections() {
  for (auto it = connections_.begin(); it != connections_.end();) {
    const auto& connection = it->second;

    if (connection.has_timed_out()) {
      std::cout << connection.name() << " timed out, connection terminated" << "\n";
      queue_->Push(Response(Request::Leave, it->first));
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
    if (!packages.Verify()) {
      std::cout << "unknown package ignored" << std::endl;
      continue;
    }
    const uint64_t host_id = packages.header_.host_id();
    const auto& host_name = packages.header_.host_name();

    if (connections_.count(host_id) == 0) {
      connections_.insert(std::make_pair(host_id, Connection(packages)));
    }
    auto& connection = connections_.at(host_id);
    auto package_index = connection.VerifySequenceNumber(packages.array_[0].header_);

    if (package_index < 0) {
      // std::cout << "old package(s) ignored" << std::endl;
      continue;
    }
    if (package_index > packages.size() || package_index >= kWindowSize) {
      std::cout << host_name << " has lost too many packages, connection will be terminated" << std::endl;
      connections_.erase(host_id);
      queue_->Push(Response(Request::Leave, host_id));
      continue;
    }
    std::vector<Package> package_vector;

    for (auto i = package_index; i >= 0; --i) {
      package_vector.push_back(packages.array_[i]);
    }
    for (const auto& package : package_vector) {
      bool process_request = true;
      const auto& header = package.header_;

      if (!header.Verify()) {
        std::cout << "Unknown package signature - package ignored" << std::endl;
        continue;
      }
      switch (header.request()) {
        case Request::Join:
          process_request = !connection.has_joined();
          connection.SetHasJoined();
          break;
        case Request::Leave:
          if (!connection.has_joined()) {
            std::cout << "Error: not joined" << std::endl;
          }
          connections_.erase(host_id);
          break;
        case Request::HeartBeat:
          process_request = false;
          break;
        default:
          break;
      }
      connection.Update(header);
      if (process_request) {
        queue_->Push(Response(packages.header_, package));
      }
    }
  }
}

} // namespace network
