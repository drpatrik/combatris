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

class Listener final {
 public:
  Listener() : cancelled_(false) {
    cancelled_.store(false, std::memory_order_release);
    queue_ = std::make_unique<ThreadSafeQueue<Package>>();
    thread_ = std::make_unique<std::thread>(std::bind(&Listener::Run, this));
  }

  Listener(const Listener&) = delete;

  virtual ~Listener() noexcept { Cancel(); }

  inline bool packages_available() const { return queue_->size() > 0; }

  inline Package NextPackage() { return queue_->Pop(); }

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
  struct ConnectionData {
    explicit ConnectionData(uint32_t sequence_nr) : sequence_nr_(sequence_nr) {
      state_ = GameState::Waiting;
      timestamp_ = utility::time_in_ms();
    }

    void UpdateTime() {
      ms_since_last_call_ = utility::time_in_ms() - timestamp_;
      timestamp_ = utility::time_in_ms();
    }

    void UpdateState(Request request, const Payload& payload) {
      if (Request::ProgressUpdate == request) {
        state_ = payload.state();
      }
      UpdateTime();
    }

    void SetState(GameState state) { state_ = state; }

    uint32_t sequence_nr_ = 0;
    GameState state_ = GameState::Idle;
    size_t timestamp_;
    size_t ms_since_last_call_ = 0;
  };

  void Run();

  bool VerifySequenceNumber(ConnectionData& connection_data, const Header& header);

  std::unordered_map<std::string, ConnectionData> connections_;
  std::unique_ptr<ThreadSafeQueue<Package>> queue_;
  std::atomic<bool> cancelled_;
  std::unique_ptr<std::thread> thread_;
};

} // namespace network
