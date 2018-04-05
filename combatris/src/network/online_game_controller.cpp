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

    package.header_ = Header(Request::HeartBeat);

    queue->Push(package);
  }
}

Package CreatePackage(Request request) {
  Package package;

  package.header_ = Header(request);

  return package;
}

} // namespace

OnlineGameController::OnlineGameController(ListenerInterface* listener_if) : listener_if_(listener_if) {
  Startup();
  our_hostname_ = GetHostName();
  cancelled_.store(false, std::memory_order_release);
  queue_ = std::make_shared<ThreadSafeQueue<Package>>();
  listener_ = std::make_unique<Listener>();
  send_thread_ = std::make_unique<std::thread>(std::bind(&OnlineGameController::Run, this));
  heartbeat_thread_ = std::make_unique<std::thread>(HeartbeatController, std::ref(cancelled_), queue_);
}

OnlineGameController::~OnlineGameController() {
  Cancel();
  Cleanup();
}

void OnlineGameController::Join() { queue_->Push(CreatePackage(Request::Join)); }

void OnlineGameController::Leave()  { queue_->Push(CreatePackage(Request::Leave)); }

void OnlineGameController::ResetCounter() { queue_->Push(CreatePackage(Request::ResetCounter)); }

void OnlineGameController::StartGame() { queue_->Push(CreatePackage(Request::StartGame)); }

void OnlineGameController::SendUpdate(size_t lines, size_t score, size_t level, size_t garbage) {
  auto package = CreatePackage(Request::ProgressUpdate);

  package.payload_ = Payload(lines, score, level, garbage, game_state_);
  queue_->Push(package);
}

void OnlineGameController::Dispatch() {
  if (nullptr == listener_if_) {
    return;
  }
  while (listener_->packages_available()) {
    auto package = listener_->NextPackage();

    switch (package.header_.request()) {
      case Request::Join:
        if (package.header_.host_name() == our_hostname_) {
          game_state_ = GameState::Waiting;
        }
        listener_if_->Join(package.header_.host_name());
        if (package.header_.host_name() != our_hostname_) {
          listener_if_->StartCounter();
        }
        break;
      case Request::Leave:
        if (package.header_.host_name() == our_hostname_) {
          game_state_ = GameState::Idle;
        }
        listener_if_->Leave(package.header_.host_name());
        break;
      case Request::ResetCounter:
        listener_if_->ResetCounter();
        break;
      case Request::StartGame:
        if (package.header_.host_name() == our_hostname_) {
          game_state_ = GameState::Playing;
        }
        listener_if_->StartGame(package.header_.host_name());
        break;
      case ProgressUpdate:
        if (package.header_.host_name() == our_hostname_) {
          game_state_ = package.payload_.state();
        }
        listener_if_->Update(package.header_.host_name(), package.payload_.lines(), package.payload_.score(),
                          package.payload_.level(), package.payload_.state());
        break;
      default:
        break;
    }
  }
}

void OnlineGameController::Run() {
  uint32_t sequence_nr = 0;
  network::UDPClient client(GetBroadcastIP(), GetPort());

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
    package.header_.SetHostName(client.host_name());
    package.header_.SetSeqenceNr(sequence_nr);
    package.payload_.SetState(game_state_);
    if (package.header_.request() != Request::HeartBeat) {
      std::cout << "Sending: " << client.host_name() << " - " << ToString(package.header_.request()) << std::endl;
    }
    client.Send(&package, sizeof(package));
    sequence_nr++;
  }
}

} // namespace network
