#include "network/multiplayer_controller.h"

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
  our_host_name_ = GetHostName();
  our_host_id_ = std::hash<std::string>{}(our_host_name_);
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
  std::cout << lines << std::endl;
  package.payload_ = Payload(0, 0, 0, 0, 0, static_cast<uint8_t>(lines), GameState::None);
  send_queue_->Push(package);
}

void MultiPlayerController::SendUpdate(uint64_t host_id) {
  auto package = CreatePackage(Request::ProgressUpdate, GameState::None);

  package.payload_.SetKnockoutBy(host_id);
  send_queue_->Push(package);
}

void MultiPlayerController::SendUpdate(GameState state) { send_queue_->Push(CreatePackage(Request::ProgressUpdate, state)); }

void MultiPlayerController::SendUpdate(int lines, int lines_sent, int score, int ko, int level) {
  auto package = CreatePackage(Request::ProgressUpdate);

  package.payload_ = Payload(static_cast<uint16_t>(lines), static_cast<uint16_t>(lines_sent), score, static_cast<uint8_t>(ko),
                             static_cast<uint8_t>(level), 0, GameState::None);
  send_queue_->Push(package);
}

void MultiPlayerController::Dispatch() {
  if (nullptr == listener_if_) {
    return;
  }
  while (listener_->packages_available()) {
    auto response = listener_->NextPackage();
    const auto& host_name = response.host_name_;
    const auto host_id = response.host_id_;
    const auto& payload = response.payload_;

    switch (response.request_) {
      case Request::Join:
        if (listener_if_->GotJoin(host_name, host_id)) {
          listener_if_->GotUpdate(host_id, 0, 0, 0, 0, 0, payload.state());
        }
        break;
      case Request::Leave:
        listener_if_->GotLeave(host_id);
        break;
      case Request::NewGame:
        listener_if_->GotNewGame(host_id);
        listener_if_->GotUpdate(host_id, 0, 0, 0, 0, 0, payload.state());
        break;
      case Request::StartGame:
        if (IsUs(host_id)) {
          listener_if_->GotStartGame();
        }
        listener_if_->GotUpdate(host_id, 0, 0, 0, 0, 0, payload.state());
        break;
      case Request::ProgressUpdate:
        if (payload.lines_got() > 0) {
          if (!IsUs(host_id)) {
            listener_if_->GotLines(host_id, payload.lines_got());
          }
          break;
        } else if (payload.knocked_out_by() != 0) {
          if (IsUs(host_id)) {
            listener_if_->GotKnockedOutBy(payload.knocked_out_by());
          }
          break;
        }
        listener_if_->GotUpdate(host_id, payload.lines(), payload.lines_sent(), payload.score(), payload.ko(),
                                payload.level(), payload.state());
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
