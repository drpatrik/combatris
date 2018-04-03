#include "network/protocol.h"
#include "network/udp_client_server.h"

#include <thread>
#include <future>
#include <iostream>
#include "catch.hpp"

namespace {

size_t kHeartBeats = 5;
int kWaitTime = 100;

} // namespace

using namespace network;

// This test might fail since there is no guarantee for a UDP
// to be received

void server(std::promise<bool> started, std::promise<int> recieved_broadcasts) {
  Server server(GetPort());
  Header header;
  size_t n = 0;

  started.set_value(true);
  do {
    header = {};
    auto size = server.Receive(&header, sizeof(Header), kWaitTime);
    n +=  (size > 0);
  } while (!(header == Request::Leave));
  recieved_broadcasts.set_value(n);
}

TEST_CASE("ClientServerTest") {
  std::promise<bool> server_started;
  std::future<bool> result_server_started{ server_started.get_future() };
  std::promise<int> recieved_broadcasts;
  std::future<int> result_recieved_broadcasts{ recieved_broadcasts.get_future() };
  std::thread server_thread{ server, std::move(server_started), std::move(recieved_broadcasts) };
  Client client(GetBroadcastIP(), GetPort());

  result_server_started.get();
  for (size_t n = 0; n < kHeartBeats; n++) {
    Header header(client.host_name(), Request::HeartBeat, n);
    client.Send(&header, sizeof(header));
    std::this_thread::sleep_for(std::chrono::milliseconds(kWaitTime));
  }
  Header header(client.host_name(), Request::Leave, 6);

  client.Send(&header, sizeof(header));

  int result = result_recieved_broadcasts.get();

  server_thread.join();

  REQUIRE(result == kHeartBeats + 1);
}

void echo_server() {

}

TEST_CASE("RunServer", "[!hide]") {
  Server server(GetPort());

  for(;;) {
    Header header{};
    auto size = server.Receive(&header, sizeof(Header), kWaitTime);

    if (size > 0) {
      std::cout << header.host_name() << "," << ToString(header.request()) << std::endl;
    }
  }

}

TEST_CASE("RunClient", "[!hide]") {
  Client client(GetBroadcastIP(), GetPort());

  for (size_t n = 0; n < kHeartBeats; n++) {
    Header header(client.host_name(), Request::HeartBeat, n);
    client.Send(&header, sizeof(header));
    std::cout << header.host_name() << "," << ToString(header.request()) << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(kWaitTime));
  }
  Header header(client.host_name(), Request::Leave, 6);

  client.Send(&header, sizeof(header));
}
