#include "network/multiplayer_controller.h"

#include <chrono>
#include <vector>
#include <iostream>

namespace network {

namespace {

void HeartbeatController(std::atomic<bool>& quit, std::shared_ptr<ThreadSafeQueue<Package>> queue) {
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(kHeartBeatInterval));

    if (quit.load(std::memory_order_acquire)) {
      return;
    }
    queue->Push(CreatePackage(Request::HeartBeat));
  }
}

} // namespace

MultiPlayerController::MultiPlayerController(ListenerInterface* listener_if) : listener_if_(listener_if) {
  Startup();
  our_hostname_ = GetHostName();
  cancelled_.store(false, std::memory_order_release);
  send_queue_ = std::make_shared<ThreadSafeQueue<Package>>();
  listener_ = std::make_unique<Listener>();
  send_thread_ = std::make_unique<std::thread>(std::bind(&MultiPlayerController::Run, this));
}

MultiPlayerController::~MultiPlayerController() {
  Leave();
  do {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  } while (send_queue_->size() > 0);
  Cancel();
  Cleanup();
}

void MultiPlayerController::Join(GameState state) {
  if (!heartbeat_thread_) {
    heartbeat_thread_ = std::make_unique<std::thread>(HeartbeatController, std::ref(cancelled_), send_queue_);
  }
  send_queue_->Push(CreatePackage(Request::Join, state));
}

void MultiPlayerController::Leave()  {
  heartbeat_thread_.release();
  send_queue_->Push(CreatePackage(Request::Leave, GameState::Idle));
}

void MultiPlayerController::NewGame() { send_queue_->Push(CreatePackage(Request::NewGame, GameState::Waiting)); }

void MultiPlayerController::StartGame() { send_queue_->Push(CreatePackage(Request::StartGame, GameState::Playing)); }

void MultiPlayerController::SendUpdate(int lines) {
  auto package = CreatePackage(Request::ProgressUpdate);

  package.payload_ = Payload(0, 0, 0, 0, static_cast<uint8_t>(lines), GameState::None);
  send_queue_->Push(package);
}

void MultiPlayerController::SendUpdate(const std::string& name) {
  auto package = CreatePackage(Request::ProgressUpdate, GameState::None);

  package.payload_.SetKnockoutBy(name);
  send_queue_->Push(package);
}

void MultiPlayerController::SendUpdate(GameState state) { send_queue_->Push(CreatePackage(Request::ProgressUpdate, state)); }

void MultiPlayerController::SendUpdate(int lines, int lines_sent, int score, int level) {
  auto package = CreatePackage(Request::ProgressUpdate);

  package.payload_ = Payload(static_cast<uint16_t>(lines), static_cast<uint16_t>(lines_sent), score, static_cast<uint8_t>(level), 0, GameState::None);
  send_queue_->Push(package);
}

void MultiPlayerController::Dispatch() {
  if (nullptr == listener_if_) {
    return;
  }
  while (listener_->packages_available()) {
    auto [host_name, package] = listener_->NextPackage();

    switch (package.header_.request()) {
      case Request::Join:
        if (listener_if_->GotJoin(host_name)) {
          listener_if_->GotUpdate(host_name, 0, 0, 0, 0, package.payload_.state());
        }
        break;
      case Request::Leave:
        listener_if_->GotLeave(host_name);
        break;
      case Request::NewGame:
        listener_if_->GotNewGame(host_name);
        listener_if_->GotUpdate(host_name, 0, 0, 0, 0, package.payload_.state());
        break;
      case Request::StartGame:
        if (host_name == our_hostname_) {
          listener_if_->GotStartGame();
        }
        listener_if_->GotUpdate(host_name, 0, 0, 0, 0, package.payload_.state());
        break;
      case Request::ProgressUpdate:
        if (package.payload_.lines_got() > 0) {
          listener_if_->GotLines(host_name, package.payload_.lines_got());
        } else if (package.payload_.knocked_out_by() != 0) {
          listener_if_->GotKnockedOutBy(package.payload_.knocked_out_by());
        } else {
          listener_if_->GotUpdate(host_name, package.payload_.lines(), package.payload_.lines_sent(),
                                  package.payload_.score(), package.payload_.level(), package.payload_.state());
        }
        break;
      default:
        break;
    }
  }
}

void MultiPlayerController::Run() {
  uint32_t sequence_nr = 0;
  UDPClient client(GetBroadcastIP(), GetPort());
  auto time_since_last_package = utility::time_in_ms();

  std::cout << "Broadcast IP: " << GetBroadcastIP() << ", Port: " << GetPort() << std::endl;

  for (;;) {
    if (cancelled_.load(std::memory_order_acquire)) {
      break;
    }
    Package package;

    if (!send_queue_->Pop(package)) {
      break;
    }
    if (cancelled_.load(std::memory_order_acquire)) {
      break;
    }
    if (package.header_.request() == Request::HeartBeat && (utility::time_in_ms() - time_since_last_package) < kHeartBeatInterval) {
      continue;
    }
    time_since_last_package = utility::time_in_ms();

    package.header_.SetSeqenceNr(sequence_nr);
    sequence_nr++;
    if (sliding_window_.size() == kWindowSize) {
      sliding_window_.pop_back();
    }
    sliding_window_.push_front(package);

    Packages packages(client.host_name(), sliding_window_.size());

    std::copy(std::begin(sliding_window_), std::end(sliding_window_), packages.array_);
    client.Send(&packages, sizeof(packages));
  }
}

} // namespace network
