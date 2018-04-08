#include "network/multiplayer_controller.h"

#include <chrono>
#include <iostream>

namespace network {

namespace {

const int kHeartBeatInterval = 1000;

void HeartbeatController(std::atomic<bool>& quit, std::shared_ptr<ThreadSafeQueue<Package>> queue) {
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(kHeartBeatInterval));

    if (quit.load(std::memory_order_acquire)) {
      return;
    }
    Package package;

    package.header_ = PackageHeader(Request::HeartBeat);

    queue->Push(package);
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
  do {
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
  } while (send_queue_->size() > 0);
  Cancel();
  Cleanup();
}

void MultiPlayerController::Join() {
  if (!heartbeat_thread_) {
    heartbeat_thread_ = std::make_unique<std::thread>(HeartbeatController, std::ref(cancelled_), send_queue_);
  }
  send_queue_->Push(CreatePackage(Request::Join));
}

void MultiPlayerController::Leave()  {
  heartbeat_thread_.release();
  send_queue_->Push(CreatePackage(Request::Leave));
}

void MultiPlayerController::Play() {
  if (GameState::Idle == game_state_) {
    return;
  }
  send_queue_->Push(CreatePackage(Request::Play));
}

void MultiPlayerController::ResetCountDown() {
  send_queue_->Push(CreatePackage(Request::ResetCountDown));
}

void MultiPlayerController::StartGame() {
  send_queue_->Push(CreatePackage(Request::StartGame));
}

void MultiPlayerController::SendUpdate(size_t lines, size_t score, size_t level, size_t garbage) {
  auto package = CreatePackage(Request::ProgressUpdate);

  package.payload_ = Payload(lines, score, level, garbage, game_state_);
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
        if (host_name == our_hostname_) {
          game_state_ = GameState::Idle;
        }
        listener_if_->Join(host_name);
        break;
      case Request::Leave:
        if (host_name == our_hostname_) {
          game_state_ = GameState::Idle;
        }
        listener_if_->Leave(host_name);
        break;
      case Request::Play:
        if (host_name == our_hostname_) {
          game_state_ = GameState::Waiting;
          ResetCountDown();
        }
        break;
      case Request::ResetCountDown:
        listener_if_->ResetCountDown();
        break;
      case Request::StartGame:
        if (host_name == our_hostname_) {
          game_state_ = GameState::Playing;
        }
        listener_if_->StartGame(host_name);
        break;
      case ProgressUpdate:
        listener_if_->Update(host_name, package.payload_.lines(), package.payload_.score(),
                          package.payload_.level(), package.payload_.state());
        if (package.payload_.garbage() != 0) {
          listener_if_->GotLines(host_name, package.payload_.garbage());
        }
        if (host_name == our_hostname_) {
          game_state_ = package.payload_.state();
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
    package.header_.SetSeqenceNr(sequence_nr);
    package.payload_.SetState(game_state_);
    if (package.header_.request() != Request::HeartBeat) {
      std::cout << "Sending: " << client.host_name() << " - " << ToString(package.header_.request()) << std::endl;
    }
    if (sliding_window_.size() == kWindowSize) {
      sliding_window_.pop_back();
    }
    sliding_window_.push_front(package);

    Packages packages(client.host_name(), sliding_window_.size());

    std::copy(std::begin(sliding_window_), std::end(sliding_window_), packages.array_);
    client.Send(&packages, sizeof(packages));
    sequence_nr++;
  }
}

} // namespace network
