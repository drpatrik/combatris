#pragma once

#include "network/listener.h"

#include <deque>

namespace network {

class ListenerInterface {
 public:
  virtual ~ListenerInterface() {}

  virtual bool GotJoin(const std::string& name) = 0;

  virtual void GotLeave(const std::string& name) = 0;

  virtual void GotNewGame(const std::string& name) = 0;

  virtual void GotStartGame() = 0;

  virtual void GotUpdate(const std::string& name, size_t lines, size_t score, size_t level, GameState state) = 0;

  virtual void GotLines(const std::string& name, size_t lines) = 0;
};

class MultiPlayerController {
 public:
  MultiPlayerController(ListenerInterface* listener);

  ~MultiPlayerController();

  void Join(network::GameState state = GameState::Idle);

  void Leave();

  void NewGame();

  void StartGame();

  void SendUpdate(size_t garbage);

  void SendUpdate(GameState state);

  void SendUpdate(size_t lines, size_t score, size_t level);

  void Dispatch();

  const std::string& our_host_name() const { return our_hostname_; }

 protected:
  void Run();

  void Wait() {
    if (!send_thread_) {
      return;
    }
    if (send_thread_->joinable()) {
      send_thread_->join();
    }
    if (heartbeat_thread_) {
      if (heartbeat_thread_->joinable()) {
        heartbeat_thread_->join();
      }
    }
  }

  void Cancel() noexcept {
    if (!send_thread_ || cancelled_.load(std::memory_order_acquire)) {
      return;
    }
    cancelled_.store(true, std::memory_order_release);
    send_queue_->Cancel();
    listener_->Cancel();
    Wait();
  }

 private:
  std::string our_hostname_;
  std::atomic<bool> cancelled_;
  ListenerInterface* listener_if_;
  std::unique_ptr<Listener> listener_;
  std::shared_ptr<ThreadSafeQueue<Package>> send_queue_;
  std::unique_ptr<std::thread> send_thread_;
  std::unique_ptr<std::thread> heartbeat_thread_;
  std::deque<Package> sliding_window_;
};

} // namespace network
