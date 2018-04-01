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

void server(std::promise<int> result_promise) {
  Server server(GetServer(), GetPort());
  Header header;
  size_t n = 0;

  do {
    header = {};
    auto size = server.Receive(&header, sizeof(Header), kWaitTime);

    n += (header.request_ == Request::HeartBeat);
    n += (header.request_ == Request::Leave);
  } while (header.request_ != Request::Leave);
  result_promise.set_value(n);
}

TEST_CASE("ClientServerTest") {
  std::promise<int> result_promise;
  std::future<int> result_future{ result_promise.get_future() };
  std::thread server_thread{ server, std::move(result_promise) };
  Client client(GetServer(), GetPort());

  for (size_t n = 0; n < kHeartBeats; n++) {
    Header header(client.host_name(), Request::HeartBeat, n);

    client.Send(&header, sizeof(header));
    std::this_thread::sleep_for(std::chrono::milliseconds(kWaitTime));
  }
  Header header(client.host_name(), Request::Leave, 6);

  client.Send(&header, sizeof(header));

  int result = result_future.get();

  server_thread.join();

  REQUIRE(result == kHeartBeats + 1);
}
