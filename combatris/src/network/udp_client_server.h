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

const int SOCKET_TIMEOUT = -2;

class UDPClient {
 public:
  UDPClient(const std::string& broadcast_address, int port);

  UDPClient(const UDPClient&) = delete;

  ~UDPClient() noexcept;

  ssize_t Send(void* buff, size_t size);

  const std::string& host_name() const { return host_name_; }

 private:
  SOCKET socket_ = INVALID_SOCKET;
  addrinfo* addr_info_ = nullptr;
  std::string host_name_;
};

class UDPServer {
 public:
  explicit UDPServer(int port);

  UDPServer(const UDPServer&) = delete;

  ~UDPServer() noexcept;

  ssize_t Receive(void* buff, size_t max_size) { return recv(socket_, static_cast<char*>(buff), max_size, 0); }

  ssize_t Receive(void* buff, size_t max_size, int max_wait_ms);

  ssize_t Receive(void* buff, size_t max_size, sockaddr_in& from_addr, int max_wait_ms);

  const std::string& host_name() const { return host_name_; }

 private:
  SOCKET socket_ = INVALID_SOCKET;
  addrinfo* addr_info_ = nullptr;
  std::string host_name_;
};

void Startup();

void Cleanup();

std::string GetHostName();

}  // namespace network
