#include "network/online_game_controller.h"

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

OnlineGameController::OnlineGameController(ListenerInterface* listener_if) : listener_if_(listener_if) {
  Startup();
  our_hostname_ = GetHostName();
  cancelled_.store(false, std::memory_order_release);
  queue_ = std::make_shared<ThreadSafeQueue<Package>>();
  listener_ = std::make_unique<Listener>();
  send_thread_ = std::make_unique<std::thread>(std::bind(&OnlineGameController::Run, this));
}

OnlineGameController::~OnlineGameController() {
  Cancel();
  Cleanup();
}

void OnlineGameController::Join() {
  if (!heartbeat_thread_) {
    heartbeat_thread_ = std::make_unique<std::thread>(HeartbeatController, std::ref(cancelled_), queue_);
  }
  queue_->Push(CreatePackage(Request::Join));
}

void OnlineGameController::Leave()  {
  if (GameState::Idle == game_state_) {
    return;
  }
  heartbeat_thread_.release();
  queue_->Push(CreatePackage(Request::Leave));
}

void OnlineGameController::ResetCounter() {
  if (GameState::Idle == game_state_) {
    return;
  }
  queue_->Push(CreatePackage(Request::ResetCounter));
}

void OnlineGameController::StartGame() {
  if (GameState::Idle == game_state_) {
    return;
  }
  queue_->Push(CreatePackage(Request::StartGame));
}

void OnlineGameController::PlayAgain() {
  if (GameState::Idle == game_state_) {
    return;
  }
  queue_->Push(CreatePackage(Request::PlayAgain));
}

void OnlineGameController::SendUpdate(size_t lines, size_t score, size_t level, size_t garbage) {
  if (GameState::Idle == game_state_) {
    return;
  }
  auto package = CreatePackage(Request::ProgressUpdate);

  package.payload_ = Payload(lines, score, level, garbage, game_state_);
  queue_->Push(package);
}

void OnlineGameController::Dispatch() {
  if (nullptr == listener_if_) {
    return;
  }
  while (listener_->packages_available()) {
    auto [host_name, package] = listener_->NextPackage();

    switch (package.header_.request()) {
      case Request::Join:
        if (host_name == our_hostname_) {
          game_state_ = GameState::Waiting;
        }
        listener_if_->Join(host_name);
        if (host_name != our_hostname_) {
          listener_if_->StartCounter();
        }
        break;
      case Request::Leave:
        if (host_name == our_hostname_) {
          game_state_ = GameState::Idle;
        }
        listener_if_->Leave(host_name);
        break;
      case Request::ResetCounter:
        listener_if_->ResetCounter();
        break;
      case Request::StartGame:
        if (host_name == our_hostname_) {
          game_state_ = GameState::Playing;
        }
        listener_if_->StartGame(host_name);
        break;
      case Request::PlayAgain:
        if (host_name == our_hostname_) {
          game_state_ = GameState::Waiting;
        }
        listener_if_->Update(host_name, 0, 0, 0, GameState::Waiting);
        break;
      case ProgressUpdate:
        listener_if_->Update(host_name, package.payload_.lines(), package.payload_.score(),
                          package.payload_.level(), package.payload_.state());
        if (host_name == our_hostname_) {
          game_state_ = package.payload_.state();
        } else {
          listener_if_->StartCounter();
        }
        break;
      default:
        break;
    }
  }
}

void OnlineGameController::Run() {
  uint32_t sequence_nr = 0;
  UDPClient client(GetBroadcastIP(), GetPort());

  for (;;) {
    if (cancelled_.load(std::memory_order_acquire)) {
      break;
    }
    Package package;

    if (!queue_->Pop(package)) {
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
