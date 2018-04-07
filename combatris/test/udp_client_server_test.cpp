#include "network/protocol.h"
#include "network/udp_client_server.h"

#include <thread>
#include <future>
#include <iostream>
#include "catch.hpp"

namespace {

int kHeartBeats = 5;
int kWaitTime = 100;

class Initialize {
 public:
  Initialize() { network::Startup(); }
  ~Initialize() { network::Cleanup(); }
};

struct TestPackage {
  TestPackage() {}
  TestPackage(const std::string& host_name, network::Request request) {
    header_.SetHostName(host_name);
    package_header_ = network::PackageHeader(request);
  }
  network::Header header_;
  network::PackageHeader package_header_;
};

Initialize initialize;

} // namespace

using namespace network;

// This test might fail since there is no guarantee for a UDP
// to be received

void server(std::promise<bool> started, std::promise<int> recieved_broadcasts) {
  UDPServer server(GetPort());
  TestPackage package;
  size_t n = 0;

  started.set_value(true);
  do {
    package = {};
    auto size = server.Receive(&package, sizeof(package), kWaitTime);
    n +=  (size > 0);
  } while (!(package.package_header_ == Request::Leave));
  recieved_broadcasts.set_value(n);
}

TEST_CASE("ClientServerTest") {
  std::promise<bool> server_started;
  std::future<bool> result_server_started{ server_started.get_future() };
  std::promise<int> recieved_broadcasts;
  std::future<int> result_recieved_broadcasts{ recieved_broadcasts.get_future() };
  std::thread server_thread{ server, std::move(server_started), std::move(recieved_broadcasts) };
  UDPClient client(GetBroadcastIP(), GetPort());

  result_server_started.get();
  for (int n = 0; n < kHeartBeats; n++) {
    TestPackage package(client.host_name(), Request::HeartBeat);
    client.Send(&package, sizeof(package));
    std::this_thread::sleep_for(std::chrono::milliseconds(kWaitTime));
  }
  TestPackage package(client.host_name(), Request::Leave);

  client.Send(&package, sizeof(package));

  int result = result_recieved_broadcasts.get();

  server_thread.join();

  REQUIRE(result == kHeartBeats + 1);
}

TEST_CASE("RunServer", "[!hide]") {
  UDPServer server(GetPort());

  for(;;) {
    TestPackage package;
    auto size = server.Receive(&package, sizeof(package), kWaitTime);

    if (size > 0) {
      std::cout << package.header_.host_name() << "," << ToString(package.package_header_.request()) << std::endl;
    }
  }
}

TEST_CASE("RunClient", "[!hide]") {
  UDPClient client(GetBroadcastIP(), GetPort());

  for (int n = 0; n < kHeartBeats; n++) {
    TestPackage package(client.host_name(), Request::HeartBeat);
    client.Send(&package, sizeof(package));
    std::cout << package.header_.host_name() << "," << ToString(package.package_header_.request()) << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(kWaitTime));
  }
  TestPackage package(client.host_name(), Request::Leave);

  client.Send(&package, sizeof(package));
}
