#pragma once

#if defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <BaseTsd.h>
using ssize_t = SSIZE_T;
#else
#include <sys/socket.h>
#include <netdb.h>
using SOCKET = int;
const int INVALID_SOCKET = -1;
const int SOCKET_ERROR = -1;
#endif

#include <string>

namespace network {

class Client {
 public:
  Client(const Client&) = delete;

  Client(const std::string& broadcast_address, int port);

  ~Client() noexcept;

  ssize_t Send(void* buff, size_t size);

  const std::string& host_name() const { return host_name_; }

 private:
  SOCKET socket_ = INVALID_SOCKET;
	addrinfo* addr_info_ = nullptr;
  std::string host_name_;
};

class Server {
 public:
  Server(const Server&) = delete;

  Server(int port);

  ~Server() noexcept;

  ssize_t Receive(void* buff, size_t max_size) { return recv(socket_, static_cast<char*>(buff), max_size, 0); }

  ssize_t Receive(void* buff, size_t max_size, int max_wait_ms);

  const std::string& host_name() const { return host_name_; }

 private:
  SOCKET socket_ = INVALID_SOCKET;
  addrinfo* addr_info_ = nullptr;
  std::string host_name_;
};

void Startup();

void Cleanup();

}  // namespace network
