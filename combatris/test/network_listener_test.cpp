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
  ReliablePackage reliable_package(client.host_name(), sliding_window.size());

  std::copy(std::begin(sliding_window), std::end(sliding_window), reliable_package.package_.packages_);
  client.Send(&reliable_package, sizeof(reliable_package));
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
  auto rsp = listener.NextPackage();

  REQUIRE(std::hash<std::string>{}(expected_host_name) == rsp.host_id_);
  REQUIRE(expected_request == rsp.request_);
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
  sliding_window.push_front(PreparePackage(1, Request::StartGame));
  Send(sliding_window, client);
  CheckResponse(listener, client.host_name(), Request::StartGame);
  sliding_window.push_front(PreparePackage(2, Request::ProgressUpdate));
  sliding_window.push_front(PreparePackage(3, Request::ProgressUpdate));
  sliding_window.push_front(PreparePackage(4, Request::HeartBeat));
  Send(sliding_window, client);
  CheckResponse(listener, client.host_name(), Request::ProgressUpdate);
  CheckResponse(listener, client.host_name(), Request::ProgressUpdate);

  sliding_window.clear();
  sliding_window.push_front(PreparePackage(0, Request::Join));
  sliding_window.push_front(PreparePackage(1, Request::StartGame));
  Send(sliding_window, client);
  REQUIRE_FALSE(WaitForPackage(listener));

  sliding_window.clear();
  sliding_window.push_front(PreparePackage(0, Request::Join));
  sliding_window.push_front(PreparePackage(1, Request::StartGame));
  sliding_window.push_front(PreparePackage(2, Request::ProgressUpdate));
  Send(sliding_window, client);
  REQUIRE_FALSE(WaitForPackage(listener));

  sliding_window.clear();
  sliding_window.push_front(PreparePackage(0, Request::Join));
  sliding_window.push_front(PreparePackage(1, Request::StartGame));
  sliding_window.push_front(PreparePackage(2, Request::ProgressUpdate));
  sliding_window.push_front(PreparePackage(3, Request::ProgressUpdate));
  sliding_window.push_front(PreparePackage(4, Request::HeartBeat));
  sliding_window.push_front(PreparePackage(5, Request::NewGame));
  Send(sliding_window, client);
  CheckResponse(listener, client.host_name(), Request::NewGame);
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

TEST_CASE("TestTimeout") {
  UDPClient client(GetBroadcastIP(), GetPort());
  Listener listener;

  std::deque<Package> sliding_window;

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  sliding_window.push_front(PreparePackage(0, Request::Join));
  Send(sliding_window, client);
  CheckResponse(listener, client.host_name(), Request::Join);
  std::this_thread::sleep_for(std::chrono::milliseconds(kConnectionTimeOut + 2000));
  CheckResponse(listener, client.host_name(), Request::Leave);
}

TEST_CASE("TestJoinOutOfOrder") {
  UDPClient client(GetBroadcastIP(), GetPort());
  Listener listener;

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  std::deque<Package> sliding_window;

  sliding_window.push_front(PreparePackage(5, Request::Join));
  sliding_window.push_front(PreparePackage(6, Request::HeartBeat));
  sliding_window.push_front(PreparePackage(7, Request::HeartBeat));
  Send(sliding_window, client);
  CheckResponse(listener, client.host_name(), Request::Join);
  sliding_window.push_front(PreparePackage(8, Request::Leave));
  Send(sliding_window, client);
  CheckResponse(listener, client.host_name(), Request::Leave);
  sliding_window.clear();
  sliding_window.push_front(PreparePackage(0, Request::Join));
  sliding_window.push_front(PreparePackage(1, Request::HeartBeat));
  Send(sliding_window, client);
  CheckResponse(listener, client.host_name(), Request::Join);
}

void SendPackage(UDPClient& client, std::deque<Package>& sliding_window, const Package& package) {
  sliding_window.push_front(package);

  ReliablePackage packages("TestClient", sliding_window.size());

  std::copy(std::begin(sliding_window), std::end(sliding_window), packages.package_.packages_);

  client.Send(&packages, sizeof(packages));
}


TEST_CASE("FakeClient", "[!hide]") {
  int dummy;
  UDPClient client(GetBroadcastIP(), GetPort());

  std::deque<Package> sliding_window;

  SendPackage(client, sliding_window, PreparePackage(0, Request::Join, GameState::Idle));

  std::cout << "Press Return\n";
  std::cin >> dummy;

  SendPackage(client, sliding_window, PreparePackage(1, Request::NewGame, GameState::Waiting));

  std::cout << "Press Return\n";
  std::cin >> dummy;

  SendPackage(client, sliding_window, PreparePackage(2, Request::StartGame, GameState::Playing));

  std::cout << "Press Return\n";
  std::cin >> dummy;

  SendPackage(client, sliding_window, PreparePackage(3, Request::Leave, GameState::Waiting));
}
