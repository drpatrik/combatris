#pragma once

#include "utility/threadsafe_queue.h"
#include "network/udp_client_server.h"
#include "network/connection.h"

#include <memory>
#include <thread>
#include <functional>
#include <unordered_map>

namespace network {

class Listener final {
 public:
  struct Response {
    Response() = default;

    Response(Request request, uint64_t host_id) : request_(request), host_id_(host_id) {}

    Response(const PackageHeader& package_header, const Package& package) {
      request_ = package.header_.request();
      host_name_ = package_header.host_name();
      host_id_ = package_header.host_id();
      payload_ = package.payload_;
    }

    Response(const PackageHeader& package_header, const ProgressPackage& package) {
      request_ = Request::ProgressUpdate;
      host_name_ = package_header.host_name();
      host_id_ = package_header.host_id();
      progress_payload_ = package.payload_;
    }

    Request request_;
    std::string host_name_;
    uint64_t host_id_;
    Payload payload_;
    ProgressPayload progress_payload_;
  };

  Listener() : cancelled_(false) {
    cancelled_.store(false, std::memory_order_release);
    queue_ = std::make_unique<ThreadSafeQueue<Response>>();
    thread_ = std::make_unique<std::thread>(std::bind(&Listener::Run, this));
  }

  Listener(const Listener&) = delete;

  virtual ~Listener() noexcept { Cancel(); }

  inline bool packages_available() const { return queue_->size() > 0; }

  inline Response NextPackage() { return queue_->Pop(); }

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
  void Run();

  void TerminateTimedOutConnections();

  void HandleReliableChannel(ssize_t size, char* buffer);

  void HandleUnreliableChannel(ssize_t size, char* buffer);

  std::atomic<bool> cancelled_;
  std::unordered_map<uint64_t, Connection> connections_;
  std::unique_ptr<ThreadSafeQueue<Response>> queue_;
  std::unique_ptr<std::thread> thread_;
};

} // namespace network
