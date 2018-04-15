#pragma once

#include "utility/timer.h"
#include "utility/threadsafe_queue.h"
#include "network/protocol.h"
#include "network/udp_client_server.h"
#include "network/protocol_timing_settings.h"

#include <memory>
#include <thread>
#include <functional>
#include <unordered_map>

namespace network {

class Listener final {
 public:
  Listener() : cancelled_(false) {
    cancelled_.store(false, std::memory_order_release);
    queue_ = std::make_unique<ThreadSafeQueue<std::pair<std::string, Package>>>();
    thread_ = std::make_unique<std::thread>(std::bind(&Listener::Run, this));
  }

  Listener(const Listener&) = delete;

  virtual ~Listener() noexcept { Cancel(); }

  inline bool packages_available() const { return queue_->size() > 0; }

  inline std::pair<std::string, Package> NextPackage() { return queue_->Pop(); }

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
  struct Connection {
    Connection(uint32_t sequence_nr) : sequence_nr_(sequence_nr) { timestamp_ = utility::time_in_ms(); }

    void Update(const PackageHeader& header) {
      sequence_nr_ = header.sequence_nr();
      timestamp_ = utility::time_in_ms();
    }

    bool has_timed_out() const { return utility::time_in_ms() - timestamp_ >= kConnectionTimeOut; }

    bool has_joined() const { return has_joined_; }

    void SetJoined() { has_joined_ = true; }

    void SetLeft() { has_joined_ = false; }

    bool has_joined_ = false;
    uint32_t sequence_nr_ = 0;
    int64_t timestamp_;
  };

  void Run();

  int64_t VerifySequenceNumber(Connection& connection_data, const std::string& name, const PackageHeader& header);

  void TerminateTimedOutConnections();

  std::atomic<bool> cancelled_;
  std::unordered_map<std::string, Connection> connections_;
  std::unique_ptr<ThreadSafeQueue<std::pair<std::string, Package>>> queue_;
  std::unique_ptr<std::thread> thread_;
};

} // namespace network
