#include "network/listener.h"

namespace {
  const int kWaitTime = 500;
} // namespace

namespace network {

bool Listener::VerifySequenceNumber(Listener::ConnectionData& connection_data, const Header& header) {
  auto status = true;
  auto new_sequence_nr_ = header.sequence_nr();
  auto old_sequence_nr_ = connection_data.sequence_nr_;
  auto gap = std::abs(int64_t(new_sequence_nr_) - int64_t(old_sequence_nr_));

  if (gap > 1) {
    std::cout << "Gap detected: " << header.host_name() << ", got - " << new_sequence_nr_ << ", expected - "
              << old_sequence_nr_ + 1 << std::endl;
    status = false;
  }
  connection_data.sequence_nr_ = new_sequence_nr_;

  return status;
}

void Listener::Run() {
  UDPServer server(GetPort());

  for (;;) {
    if (cancelled_.load(std::memory_order_acquire)) {
      break;
    }
    Package package{};
    auto size = server.Receive(&package, sizeof(package), kWaitTime);

    if (size == SOCKET_ERROR) {
      continue;
    }
    if (cancelled_.load(std::memory_order_acquire)) {
      break;
    }
    if (size < static_cast<ssize_t>(sizeof(Package))) {
      std::cout << "Incomplete package - " << size << std::endl;
      continue;
    }
    auto header = package.header_;
    auto payload = package.payload_;

    if (!header.VerifyHeader()) {
      std::cout << "Unknown package ignored" << std::endl;
      continue;
    }
    auto host_name = header.host_name();

    if (header.request() == Request::HeartBeat) {
      if (connections_.count(host_name) > 0) {
        auto& connection_data = connections_.at(host_name);

        VerifySequenceNumber(connection_data, package.header_);
        connection_data.UpdateTime();
      }
      continue;
    }
    if (header.request() == Request::Join) {
      connections_.insert(std::make_pair(host_name, ConnectionData(header.sequence_nr())));
    }
    if (connections_.count(host_name) == 0) {
      continue;
    }
    auto& connection_data = connections_.at(host_name);

    switch (header.request()) {
      case Request::Join:
        std::cout << "Client: " << host_name << " joined" << std::endl;
        break;
      case Request::Leave:
        connections_.erase(host_name);
        std::cout << "Client: " << host_name << " left" << std::endl;
        break;
      case Request::StartGame:
        connection_data.SetState(GameState::Playing);
        break;
      default:
        break;
    }
    VerifySequenceNumber(connection_data, header);

    connection_data.UpdateState(header.request(), payload);
    queue_->Push(package);
  }
}

} // namespace network
