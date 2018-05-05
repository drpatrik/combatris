#pragma once

#include "network/listener.h"

namespace network {

class ListenerInterface {
 public:
  virtual ~ListenerInterface() {}

  virtual bool GotJoin(const std::string& display_name, uint64_t host_id) = 0;

  virtual void GotLeave(uint64_t host_id) = 0;

  virtual void GotNewGame(uint64_t host_id) = 0;

  virtual void GotStartGame() = 0;

  virtual void GotUpdate(uint64_t host_id, int lines, int lines_sent, int score, int ko, int level, GameState state) = 0;

  virtual void GotLines(uint64_t host_id, int lines) = 0;

  virtual void GotPlayerKnockedOut() = 0;
};

class MultiPlayerController {
 public:
  MultiPlayerController(ListenerInterface* listener);

  ~MultiPlayerController();

  void Join(network::GameState state = GameState::Idle);

  void Leave();

  void NewGame();

  void StartGame();

  void SendUpdate(int lines);

  void SendUpdate(uint64_t knockout_by);

  void SendUpdate(GameState state);

  void SendUpdate(int lines, int lines_sent_, int score, int ko, int level);

  void Dispatch();

  inline bool IsUs(uint64_t host_id) const { return host_id == our_host_id_; }

  inline const std::string& our_host_name() const { return our_host_name_; }

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
    listener_->Cancel();
    send_queue_->Cancel();
    Wait();
  }

 private:
  uint64_t our_host_id_;
  std::string our_host_name_;
  std::atomic<bool> cancelled_;
  ListenerInterface* listener_if_;
  std::unique_ptr<Listener> listener_;
  std::shared_ptr<ThreadSafeQueue<Package>> send_queue_;
  std::unique_ptr<std::thread> send_thread_;
  std::unique_ptr<std::thread> heartbeat_thread_;
};

} // namespace network
