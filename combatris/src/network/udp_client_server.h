#pragma once

#if defined(_WIN64)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <BaseTsd.h>
#include <process.h>

using ssize_t = SSIZE_T;

inline int GetPID() { return _getpid(); }

#else

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

inline int GetPID() { return getpid(); }

using SOCKET = int;
const int INVALID_SOCKET = -1;
const int SOCKET_ERROR = -1;

#endif

#include <string>

namespace network {

const int SOCKET_TIMEOUT = -2;

std::string GetHostName();
inline uint64_t CreateUniqueID(const std::string& name) { return std::hash<std::string>{}(name + std::to_string(GetPID()));}

const std::string kHostName = GetHostName();
const uint64_t kHostID = CreateUniqueID(kHostName);

std::string GetBroadcastAddress();

int GetPort();

void Startup();

void Cleanup();

class UDPClient {
 public:
  UDPClient(const std::string& broadcast_address, int port);

  UDPClient(const UDPClient&) = delete;

  ~UDPClient() noexcept;

  ssize_t Send(void* buff, size_t size);

  inline const std::string& host_name() const { return kHostName; }

  inline uint64_t host_id() const { return kHostID; }

 private:
  SOCKET socket_ = INVALID_SOCKET;
  addrinfo* addr_info_ = nullptr;
};

class UDPServer {
 public:
  explicit UDPServer(int port);

  UDPServer(const UDPServer&) = delete;

  ~UDPServer() noexcept;

  ssize_t Receive(void* buff, size_t max_size) { return recv(socket_, static_cast<char*>(buff), static_cast<int>(max_size), 0); }

  ssize_t Receive(void* buff, size_t max_size, int max_wait_ms);

  ssize_t Receive(void* buff, size_t max_size, sockaddr_in& from_addr, int max_wait_ms);

 private:
  SOCKET socket_ = INVALID_SOCKET;
  addrinfo* addr_info_ = nullptr;
};

}  // namespace network
