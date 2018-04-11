#include "network/listener.h"

#include <deque>
#include <iostream>
#include "catch.hpp"

using namespace network;

Package PreparePackage(uint32_t sn, Request request, GameState state = GameState::Idle) {
  Package package;

  package.header_.SetSeqenceNr(sn);
  package.header_.SetRequest(request);
  package.payload_.SetState(state);

  return package;
}

void Send(const std::deque<Package>& sliding_window, UDPClient& client) {
  Packages packages(client.host_name(), sliding_window.size());

  std::copy(std::begin(sliding_window), std::end(sliding_window), packages.array_);
  client.Send(&packages, sizeof(packages));
}

bool WaitForPackage(Listener& listener) {
  for (int i = 0;i < 2; ++i) {
    if (listener.packages_available()) {
      return true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  return false;
}

void CheckResponse(Listener& listener, const std::string& expected_host_name, Request expected_request) {
  REQUIRE(WaitForPackage(listener));
  auto [host_name, package] = listener.NextPackage();

  REQUIRE(host_name == expected_host_name);
  REQUIRE(expected_request == package.header_.request());
}

TEST_CASE("TestDuplicatePackageDetection") {
  UDPClient client(GetBroadcastIP(), GetPort());
  Listener listener;

  std::deque<Package> sliding_window;

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  sliding_window.push_front(PreparePackage(0, Request::Join));

  Send(sliding_window, client);
  CheckResponse(listener, client.host_name(), Request::Join);

  sliding_window.push_front(PreparePackage(0, Request::Join));
  Send(sliding_window, client);
  REQUIRE_FALSE(WaitForPackage(listener));
}

TEST_CASE("TestGapDetection") {
  UDPClient client(GetBroadcastIP(), GetPort());
  Listener listener;

  std::deque<Package> sliding_window;

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  sliding_window.push_front(PreparePackage(0, Request::Join));
  Send(sliding_window, client);
  CheckResponse(listener, client.host_name(), Request::Join);

  sliding_window.push_front(PreparePackage(1, Request::ProgressUpdate));
  sliding_window.push_front(PreparePackage(2, Request::ProgressUpdate));
  sliding_window.push_front(PreparePackage(3, Request::HeartBeat));
  Send(sliding_window, client);
  CheckResponse(listener, client.host_name(), Request::ProgressUpdate);
  CheckResponse(listener, client.host_name(), Request::ProgressUpdate);

  sliding_window.clear();
  sliding_window.push_front(PreparePackage(0, Request::Join));
  sliding_window.push_front(PreparePackage(1, Request::ProgressUpdate));
  Send(sliding_window, client);
  REQUIRE_FALSE(WaitForPackage(listener));

  sliding_window.clear();
  sliding_window.push_front(PreparePackage(0, Request::Join));
  sliding_window.push_front(PreparePackage(1, Request::ProgressUpdate));
  sliding_window.push_front(PreparePackage(2, Request::ProgressUpdate));
  Send(sliding_window, client);
  REQUIRE_FALSE(WaitForPackage(listener));

  sliding_window.clear();
  sliding_window.push_front(PreparePackage(0, Request::Join));
  sliding_window.push_front(PreparePackage(1, Request::ProgressUpdate));
  sliding_window.push_front(PreparePackage(2, Request::ProgressUpdate));
  sliding_window.push_front(PreparePackage(3, Request::HeartBeat));
  sliding_window.push_front(PreparePackage(4, Request::StartGame));
  Send(sliding_window, client);
  CheckResponse(listener, client.host_name(), Request::StartGame);
}

TEST_CASE("LostTooManyPackagesDetection") {
  UDPClient client(GetBroadcastIP(), GetPort());
  Listener listener;

  std::deque<Package> sliding_window;

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  sliding_window.push_front(PreparePackage(0, Request::Join));
  Send(sliding_window, client);
  CheckResponse(listener, client.host_name(), Request::Join);

  for (int i = 1; i < kWindowSize; i++ ) {
    sliding_window.push_front(PreparePackage(i + 1, Request::ProgressUpdate));
  }
  sliding_window.front().header_.SetSeqenceNr(kWindowSize + 1);
  Send(sliding_window, client);
  CheckResponse(listener, client.host_name(), Request::Leave);
}
