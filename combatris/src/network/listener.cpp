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

template<typename From, typename A, typename B>
std::pair<A, B> CastBuffer(char* buffer) {
  From* package = reinterpret_cast<From *>(buffer);

  return std::make_pair(std::move(package->header_), std::move(package->package_));
}

void Listener::HandleReliableChannel(ssize_t size, char* buffer) {
  if (size != static_cast<ssize_t>(sizeof(ReliablePackage))) {
    std::cout << "incomplete package - " << size << std::endl;
    return;
  }
  const auto [package_header, package_array] = CastBuffer<ReliablePackage, PackageHeader, PackageArray>(buffer);
  const auto host_id = package_header.host_id();
  const auto& host_name = package_header.host_name();

  if (connections_.count(host_id) == 0) {
    connections_.insert(std::make_pair(host_id, Connection(host_name, package_array)));
  }
  auto& connection = connections_.at(host_id);
  auto package_index = connection.VerifySequenceNumber(Channel::Reliable, package_array.packages_[0].header_);

  if (package_index < 0) {
#if !defined(NDEBUG)
    std::cout << "old package(s) ignored\n";
#endif
    connection.IsAlive();
    return;
  }
  if (package_index > package_array.size() || package_index >= kWindowSize) {
    std::cout << host_name << " has lost too many packages, connection will be terminated" << std::endl;
    connections_.erase(host_id);
    queue_->Push(Response(Request::Leave, host_id));
    return;
  }
  std::vector<Package> package_vector;

  for (auto i = package_index; i >= 0; --i) {
    package_vector.push_back(package_array.packages_[i]);
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
    connection.Update(Channel::Reliable, header);
    if (process_request) {
      queue_->Push(Response(package_header, package));
    }
  }
}

void Listener::HandleUnreliableChannel(ssize_t size, char* buffer) {
  if (size != static_cast<ssize_t>(sizeof(UnreliablePackage))) {
    std::cout << "UnreliableChannel - package - " << size << std::endl;
    return;
  }
  const auto [package_header, progress_package] = CastBuffer<UnreliablePackage, PackageHeader, ProgressPackage>(buffer);
  const auto host_id = package_header.host_id();

  if (connections_.count(host_id) == 0) {
    return;
  }
  auto& connection = connections_.at(host_id);

  if (connection.VerifySequenceNumber(Channel::Unreliable, progress_package.header_ ) < 0) {
#if !defined(NDEBUG)
    std::cout << "UnreliableChannel - old package(s) ignored\n";
#endif
    connection.IsAlive();
    return;
  }
  connection.Update(Channel::Unreliable, progress_package.header_);
  queue_->Push(Response(package_header, progress_package));
}

void Listener::Run() {
  UDPServer server(GetPort());
  char buffer[2500];

  static_assert(sizeof(buffer) >= sizeof(ReliablePackage) && sizeof(buffer) >= sizeof(UnreliablePackage));

  auto last_timeout_check  = utility::time_in_ms();

  for (;;) {
    if (cancelled_.load(std::memory_order_acquire)) {
      break;
    }
    auto size = server.Receive(buffer, sizeof(buffer), kWaitForIncomingPackages);

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
    PackageHeader *header = reinterpret_cast<PackageHeader*>(buffer);

    if (size < static_cast<ssize_t>(sizeof(PackageHeader))) {
      std::cout << "incomplete package header - " << size << std::endl;
      return;
    }
    if (!header->Verify()) {
      std::cout << "Unknown package signature - package ignored" << std::endl;
      continue;
    }
    switch (header->channel()) {
      case Channel::Unreliable:
        HandleUnreliableChannel(size, buffer);
        break;
      case Channel::Reliable:
        HandleReliableChannel(size, buffer);
        break;
      default:
        std::cout << "None" << std::endl;
        break;
    }
  }
}

} // namespace network
