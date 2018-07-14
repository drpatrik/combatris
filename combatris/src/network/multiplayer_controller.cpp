#include "network/multiplayer_controller.h"

#include <iostream>
#include <deque>

namespace network {

namespace {

void HeartbeatController(std::atomic<bool>& quit, std::shared_ptr<ThreadSafeQueue<MultiPlayerController::OutgoingPackage>> queue) {
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
  send_queue_ = std::make_shared<ThreadSafeQueue<OutgoingPackage>>();
  listener_ = std::make_unique<Listener>();
  send_thread_ = std::make_unique<std::thread>(std::bind(&MultiPlayerController::Run, this));
}

MultiPlayerController::~MultiPlayerController() noexcept {
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

void MultiPlayerController::SendUpdate(int lines) { send_queue_->Push(CreatePackage(Request::SendLines, lines)); }

void MultiPlayerController::SendUpdate(uint64_t host_id) { send_queue_->Push(CreatePackage(Request::KnockedOutBy, host_id)); }

void MultiPlayerController::SendUpdate(GameState state) { send_queue_->Push(CreatePackage(Request::NewState, state)); }

void MultiPlayerController::SendUpdate(int lines, int score, int level, const MatrixState& state) {
  send_queue_->Push(CreatePackage(static_cast<uint16_t>(lines), score, static_cast<uint8_t>(level), state));
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
          listener_if_->GotNewState(host_id, payload.state());
        }
        break;
      case Request::Leave:
        listener_if_->GotLeave(host_id);
        break;
      case Request::NewGame:
        listener_if_->GotNewGame(host_id);
        listener_if_->GotNewState(host_id, payload.state());
        break;
      case Request::StartGame:
        if (IsUs(host_id)) {
          listener_if_->GotStartGame();
        }
        listener_if_->GotNewState(host_id, payload.state());
        break;
      case Request::NewState:
        listener_if_->GotNewState(host_id, payload.state());
        break;
      case Request::SendLines:
        listener_if_->GotLines(host_id, static_cast<int>(payload.value()));
        break;
      case Request::KnockedOutBy:
        listener_if_->GotPlayerKnockedOut(payload.value());
        break;
      case Request::ProgressUpdate:
        listener_if_->GotProgressUpdate(host_id, response.progress_payload_.lines(), response.progress_payload_.score(),
                                        response.progress_payload_.level(), response.progress_payload_.matrix_state());
        break;
      default:
        break;
    }
  }
}

void MultiPlayerController::Run() {
  const auto broadcast_address = GetBroadcastAddress();
  uint32_t sequence_nr_reliable = 0;
  uint32_t sequence_nr_unreliable = 0;
  std::deque<Package> sliding_window;
  UDPClient client(broadcast_address, GetPort());
  auto time_since_last_package = utility::time_in_ms();

  std::cout << "Broadcast IP: " << broadcast_address << ", Port: " << GetPort() << std::endl;

  for (;;) {
    if (cancelled_.load(std::memory_order_acquire)) {
      break;
    }
    OutgoingPackage outgoing_package;

    if (!send_queue_->Pop(outgoing_package)) {
      break;
    }
    if (cancelled_.load(std::memory_order_acquire)) {
      break;
    }
    if (Channel::Reliable == outgoing_package.channel()) {
      auto& package = outgoing_package.package_;

      if (package.header_.request() == Request::HeartBeat && (utility::time_in_ms() - time_since_last_package) < kHeartBeatInterval) {
        continue;
      }
      time_since_last_package = utility::time_in_ms();

      package.header_.SetSeqenceNr(sequence_nr_reliable);
      sequence_nr_reliable++;
      if (sliding_window.size() == kWindowSize) {
        sliding_window.pop_back();
      }
      sliding_window.push_front(package);

      ReliablePackage reliable_package(client.host_name(), sliding_window.size());

      std::copy(std::begin(sliding_window), std::end(sliding_window), reliable_package.package_.packages_);
      client.Send(&reliable_package, sizeof(reliable_package));
    } else {
      auto& package = outgoing_package.progress_package_;

      package.header_.SetSeqenceNr(sequence_nr_unreliable);
      sequence_nr_unreliable++;

      UnreliablePackage unreliable_package(client.host_name(), package);

      client.Send(&unreliable_package, sizeof(unreliable_package));
    }
  }
}

} // namespace network
