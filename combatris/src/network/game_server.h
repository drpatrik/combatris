#pragma once

#include "utility/timer.h"
#include "utility/threadsafe_queue.h"
#include "network/protocol.h"
#include "network/udp_client_server.h"

#include <cmath>
#include <memory>
#include <thread>
#include <atomic>
#include <iostream>
#include <string>
#include <unordered_map>

namespace network {

class GameServer final {
 public:
  GameServer() : cancelled_(false) {
    cancelled_.store(false, std::memory_order_release);
    queue_ = std::make_unique<ThreadSafeQueue<Package>>();
    thread_ = std::make_unique<std::thread>(std::bind(&GameServer::Run, this));
  }

  GameServer(const GameServer&) = delete;

  virtual ~GameServer() noexcept { Cancel(); }

  inline size_t size() const { return queue_->size(); }

  Package Pop() { return queue_->Pop(); }

  void Wait() {
    if (!thread_) {
      return;
    }
    if (thread_->joinable()) {
      thread_->join();
    }
  }

  void Cancel() noexcept {
    if (!thread_ || cancelled_.load(std::memory_order_acquire)) {
      return;
    }
    cancelled_.store(true, std::memory_order_release);
    queue_->Cancel();
    Wait();
  }

 private:
  struct ClientData {
    explicit ClientData(uint32_t sequence_nr) : sequence_nr_(sequence_nr) {
      timestamp_ = utility::time_in_ms();
    }

    void UpdateTime() { ms_since_last_call_ = utility::time_in_ms() - timestamp_; }

    uint32_t sequence_nr_ = 0;
    GameState state_ = GameState::None;
    size_t timestamp_;
    size_t ms_since_last_call_ = 0;
  };

  void Run() {
    const int kWaitTime = 500;

    UDPServer server(GetPort());

    for (;;) {
      if (cancelled_.load(std::memory_order_acquire)) {
        break;
      }
      Package package{};
      auto size = server.Receive(&package, sizeof(Header), kWaitTime);

      if (size == SOCKET_ERROR) {
        continue;
      }
      if (size < static_cast<ssize_t>(sizeof(Package))) {
        std::cout << "Incomplete package" << std::endl;
        continue;
      }
      std::string host_name = package.header_.host_name();

      if (clients_.count(host_name) == 0) {
        if (package.header_.request() == Request::Join) {
          std::cout << "Client: " << host_name << " joined" << std::endl;
          clients_.insert(std::make_pair(host_name, ClientData(package.header_.sequence_nr())));
        } else {
          std::cout << "Expected Join: " << host_name << std::endl;
        }
      } else {
        if (package.header_.request() == Request::Join) {
          std::cout << "Did not expect Join: " << host_name << std::endl;
        }
        auto& client_data = clients_.at(host_name);
        auto new_sequence_nr_ = package.header_.sequence_nr();
        auto old_sequence_nr_ = client_data.sequence_nr_;
        auto gap = std::abs(int64_t(new_sequence_nr_) - int64_t(old_sequence_nr_));

        if (gap > 0) {
          std::cout << "Gap detected: " << host_name << ", got - " << new_sequence_nr_ << ", expected - "
                    << old_sequence_nr_ + 1 << std::endl;
        }
        client_data.sequence_nr_ = new_sequence_nr_;

        if (package.header_.request() == Request::Leave) {
          clients_.erase(host_name);
          std::cout << "Client: " << host_name << " left" << std::endl;
        }
        client_data.UpdateTime();
      }
      queue_->Push(package);
    }
  }
  std::unordered_map<std::string, ClientData> clients_;
  std::unique_ptr<ThreadSafeQueue<Package>> queue_;
  std::atomic<bool> cancelled_;
  std::unique_ptr<std::thread> thread_;
};

} // namespace network
