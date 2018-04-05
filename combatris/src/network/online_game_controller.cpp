#include "network/online_game_controller.h"

#include <chrono>

using namespace network;

namespace {

const int kHeartBeatInterval = 1000;

void HeartbeatController(std::atomic<bool>& quit, std::shared_ptr<ThreadSafeQueue<Package>> queue) {

  std::this_thread::sleep_for(std::chrono::milliseconds(kHeartBeatInterval));

  if (quit.load(std::memory_order_acquire)) {
    return;
  }
  Package package;

  package.header_ = Header(Request::HeartBeat);

  queue->Push(package);
}

Package CreatePackage(Request request) {
  Package package;

  package.header_ = Header(request);

  return package;
}

} // namespace

OnlineGameController::OnlineGameController(ListenerInterface* listener) : listener_(listener) {
  Startup();
  our_hostname_ = GetHostName();
  cancelled_.store(false, std::memory_order_release);
  queue_ = std::make_shared<ThreadSafeQueue<Package>>();
  send_thread_ = std::make_unique<std::thread>(std::bind(&OnlineGameController::Run, this));
  heartbeat_thread_ = std::make_unique<std::thread>(HeartbeatController, std::ref(cancelled_), queue_);
}

OnlineGameController::~OnlineGameController() {
  Cleanup();
}

void OnlineGameController::Join() { queue_->Push(CreatePackage(Request::Join)); }

void OnlineGameController::Leave()  { queue_->Push(CreatePackage(Request::Leave)); }

void OnlineGameController::ResetCounter() { queue_->Push(CreatePackage(Request::ResetCounter)); }

void OnlineGameController::StartGame() { queue_->Push(CreatePackage(Request::StartGame)); }

void OnlineGameController::SendUpdate(size_t lines, size_t score, size_t level, size_t garbage) {
  Package package = CreatePackage(Request::ProgressUpdate);

  package.payload_ = Payload(lines, score, level, garbage, game_state_);
  queue_->Push(package);
}

void OnlineGameController::Dispatch() {
  if (nullptr == listener_) {
    return;
  }
  while (server_->size() > 0) {
    auto package = server_->Pop();

    switch (package.header_.request()) {
      case Request::Join:
        listener_->Join(package.header_.host_name());
        if (package.header_.host_name() != our_hostname_) {
          listener_->StartCounter();
        }
        break;
      case Request::Leave:
        listener_->Leave(package.header_.host_name());
        break;
      case Request::ResetCounter:
        listener_->ResetCounter();
        break;
      case Request::StartGame:
        listener_->StartGame(package.header_.host_name());
        break;
      case ProgressUpdate:
        listener_->Update(package.header_.host_name(),
                          package.payload_.lines(),
                          package.payload_.score(),
                          package.payload_.level(),
                          package.payload_.state());
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
    package.header_.set_host_name(client.host_name());
    package.header_.set_seqence_nr(sequence_nr);
    client.Send(&package, sizeof(package));
    sequence_nr++;
  }
}
