#pragma once

#include "utility/timer.h"
#include "utility/threadsafe_queue.h"
#include "network/protocol.h"
#include "network/udp_client_server.h"
#include "network/protocol_timing_settings.h"

#include <memory>
#include <thread>
#include <iostream>
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
    Connection(const Packages& packages) {
      for (int index = 0; index < packages.size(); ++index) {
        if (packages.array_[index].header_.request() == Request::Join) {
          start_index_ = index;
          sequence_nr_ = packages.array_[index].header_.sequence_nr();
          break;
        }
      }
      timestamp_ = utility::time_in_ms();
    }

    void Update(const std::string& name, const PackageHeader& header) {
      sequence_nr_ = header.sequence_nr();
      timestamp_ = utility::time_in_ms();
      if (is_missing_) {
        std::cout << name << " is back" << "\n";
        is_missing_ = false;
      }
    }

    bool has_timed_out(const std::string& name) {
      auto time_since_last_update = utility::time_in_ms() - timestamp_;

      if (time_since_last_update >= kConnectionMissing) {
        std::cout << name << " is missing, last update " << time_since_last_update << " ms ago\n";
        is_missing_ = true;
      }

      return time_since_last_update >= kConnectionTimeOut;
    }

    bool has_joined() const { return has_joined_; }

    void SetHasJoined() { has_joined_ = true; }

    void SetHasLeft() { has_joined_ = false; }

    bool has_joined_ = false;
    bool is_missing_ = false;
    int start_index_ = 0;
    uint32_t sequence_nr_;
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
