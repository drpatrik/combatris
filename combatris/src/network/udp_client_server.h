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

#if !defined(__APPLE__)

#include <endian.h>

inline uint64_t htonll(uint64_t value) {
  if (__BYTE_ORDER == __BIG_ENDIAN) {
    return value;
  }
  return (static_cast<uint64_t>(htonl(value)) << 32) | htonl(value >> 32);
}

inline uint64_t ntohll(uint64_t value) {
  if (__BYTE_ORDER == __BIG_ENDIAN) {
    return value;
  }
  return (static_cast<uint64_t>(ntohl(value)) << 32) | ntohl(value >> 32);
}

#endif // !__APPLE__

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
