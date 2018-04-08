#pragma once

#include "network/listener.h"

#include <deque>

namespace network {

class ListenerInterface {
 public:
  virtual ~ListenerInterface() {}

  virtual void Join(const std::string& name) = 0;

  virtual void Leave(const std::string& name) = 0;

  virtual void ResetCounter() = 0;

  virtual void StartGame(const std::string& name) = 0;

  virtual void Update(const std::string& name, size_t lines, size_t score, size_t level, GameState state) = 0;

  virtual void GotLines(const std::string& name, size_t lines) = 0;
};

class MultiPlayerController {
 public:
  MultiPlayerController(ListenerInterface* listener);

  ~MultiPlayerController();

  void Join();

  void Leave();

  void Play();

  void ResetCounter();

  void StartGame();

  void SendUpdate(size_t lines, size_t score, size_t level, size_t garbage);

  GameState State() const { return game_state_; }

  void SetState(GameState state) { game_state_ = state; }

  void Dispatch();

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
  GameState game_state_ = GameState::Idle;
  std::atomic<bool> cancelled_;
  ListenerInterface* listener_if_;
  std::unique_ptr<Listener> listener_;
  std::shared_ptr<ThreadSafeQueue<Package>> send_queue_;
  std::unique_ptr<std::thread> send_thread_;
  std::unique_ptr<std::thread> heartbeat_thread_;
  std::deque<Package> sliding_window_;
};

} // namespace listener
