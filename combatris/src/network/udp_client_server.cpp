#include "network/protocol.h"
#include "network/udp_client_server.h"

#if defined(_WIN64)
#pragma warning(disable:4267) // conversion from size_t to int
#pragma warning(disable:4100) // unreferenced formal parameters
#pragma warning(disable:4244) // SOCKET to int
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <unistd.h>
#include <endian.h>
#endif
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>

#include <iostream>

namespace {

#if defined(_WIN64)
#pragma comment(lib, "ws2_32.lib")

int get_last_error() { return WSAGetLastError();  }

std::string get_error_string(int error_code) {
  char msg[256];

  msg[0] = '\0';
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error_code,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), msg, sizeof(msg), nullptr);

  if ('\0' == msg[0]) {
    return "no message found for error code: " + std::to_string(error_code);
  }

  return msg;
}

#define close closesocket

#else
int get_last_error() { return errno; }

std::string get_error_string(int error_code) { return strerror(error_code); }
#endif

const int kPortLowerRange = 1024;
const int kPortUpperRange = 49151;

void Exit() {
  network::Cleanup();
  exit(-1);
}

void EnableBroadcast(const std::string& name, SOCKET socket) {
  int enable_broadcast = 1;

  if (setsockopt(socket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char*>(&enable_broadcast), sizeof(enable_broadcast)) < 0) {
    std::cout << name << ": setsockopt failed - " << get_error_string(get_last_error()) << std::endl;
    Exit();
  }
}

void SetCloseOnExit(const std::string& name, SOCKET socket) {
#if !defined(_WIN64)
  if (fcntl(socket, F_SETFD, FD_CLOEXEC) < 0) {
    std::cout << name << ": fcntl failed - " << get_error_string(get_last_error()) << std::endl;
    Exit();
  }
#endif
}

void VerifyAddressAndPort(const std::string& broadcast_address, int port) {
  if (broadcast_address.empty()) {
    std::cout << "Server Broadcast Broadcast_Addressess cannot be empty" << std::endl;
    Exit();
  }
  if (port < kPortLowerRange || port > kPortUpperRange) {
    std::cout << "Invalid port (" << kPortLowerRange << " <= " << port << " <= " << kPortUpperRange << std::endl;
    Exit();
  }
}

} // namespace

namespace network {

UDPClient::UDPClient(const std::string& broadcast_address, int port) {
  VerifyAddressAndPort(broadcast_address, port);

  addrinfo hints{};

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;

  auto ret_value(getaddrinfo(broadcast_address.c_str(), std::to_string(port).c_str(), &hints, &addr_info_));

  if (ret_value != 0 || nullptr == addr_info_) {
    std::cout << "UDPClient: "
              << "invalid address or port - \"" << broadcast_address << ":" << port << "\"" << std::endl;
    std::cout << "UDPClient: error message - " << get_error_string(get_last_error()) << std::endl;
    Cleanup();
    Exit();
  }

  socket_ = socket(addr_info_->ai_family, addr_info_->ai_socktype, addr_info_->ai_protocol);

  if (socket_ < 0) {
    std::cout << "UDPClient: could not create socket for -  \"" << broadcast_address << ":" << port << "\"" << std::endl;
    std::cout << "UDPClient: error message - " << get_error_string(get_last_error()) << std::endl;
    Exit();
  }
  SetCloseOnExit("UDPClient", socket_);
  EnableBroadcast("UDPClient", socket_);

  host_name_ = GetHostName();
}

UDPClient::~UDPClient() noexcept {
  if (addr_info_ != nullptr) {
    freeaddrinfo(addr_info_);
  }
  if (socket_ != -1) {
    close(socket_);
  }
}

ssize_t UDPClient::Send(void* buff, size_t size) {
  auto ret_value = sendto(socket_, static_cast<char*>(buff), size, 0, addr_info_->ai_addr, addr_info_->ai_addrlen);

  if (ret_value == -1) {
    std::cout << "UDPClient::Send error message: " << get_error_string(get_last_error()) << std::endl;
  }

  return ret_value;
}

UDPServer::UDPServer(int port) {
  const std::string kBroadcastAddress = "0.0.0.0";

  VerifyAddressAndPort(kBroadcastAddress, port);
  addrinfo hints{};

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;

  auto ret_value(getaddrinfo(kBroadcastAddress.c_str(), std::to_string(port).c_str(), &hints, &addr_info_));

  if (ret_value != 0 || nullptr == addr_info_) {
    std::cout << "UDPServer: " << "invalid address or port - \"" << kBroadcastAddress << ":" << port << "\"" << std::endl;
    std::cout << "UDPServer: error message - " << get_error_string(get_last_error()) << std::endl;
    Exit();
  }
  socket_ = socket(addr_info_->ai_family, addr_info_->ai_socktype, addr_info_->ai_protocol);

  if (socket_ == INVALID_SOCKET) {
    std::cout << "UDPServer: could not create socket for - \"" << kBroadcastAddress << ":" << port << "\"" << std::endl;
    std::cout << "UDPServer: error message: - " << get_error_string(get_last_error()) << std::endl;
    Exit();
  }
  SetCloseOnExit("UDPServer", socket_);
  EnableBroadcast("UDPServer", socket_);

  ret_value = bind(socket_, addr_info_->ai_addr, addr_info_->ai_addrlen);

  if (ret_value != 0) {
    std::cout << "UDPServer: could not bind socket with - \"" << kBroadcastAddress << ":" << port << "\" " << std::endl;
    std::cout << "UDPServer: error message - " << get_error_string(get_last_error()) << std::endl;
  }
  host_name_ = GetHostName();
}

UDPServer::~UDPServer() noexcept {
  if (addr_info_ != nullptr) {
    freeaddrinfo(addr_info_);
  }
  if (socket_ != INVALID_SOCKET) {
    close(socket_);
  }
}

ssize_t UDPServer::Receive(void* buff, size_t max_size, int max_wait_ms) {
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(socket_, &fds);

  timeval timeout{};
  timeout.tv_sec = max_wait_ms / 1000;
  timeout.tv_usec = (max_wait_ms % 1000) * 1000;
  const auto ret_val(select(socket_ + 1, &fds, nullptr, &fds, &timeout));

  if (ret_val == SOCKET_ERROR) {
    std::cout << "UDPServer::Receive error message - " << get_error_string(get_last_error()) << std::endl;
    return SOCKET_ERROR;
  }
  if (ret_val > 0) {
    auto size = recv(socket_, static_cast<char*>(buff), max_size, 0);

    if (size == SOCKET_ERROR) {
      std::cout << "UDPServer::Receive error message - " << get_error_string(get_last_error()) << std::endl;
    }

    return size;
  }
  return SOCKET_TIMEOUT;
}

ssize_t UDPServer::Receive(void* buff, size_t max_size, sockaddr_in& from_addr, int max_wait_ms) {
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(socket_, &fds);

  timeval timeout{};
  timeout.tv_sec = max_wait_ms / 1000;
  timeout.tv_usec = (max_wait_ms % 1000) * 1000;
  const auto ret_val(select(socket_ + 1, &fds, nullptr, &fds, &timeout));

  if (ret_val == SOCKET_ERROR) {
    std::cout << "UDPServer::Receive error message - " << get_error_string(get_last_error()) << std::endl;
    return SOCKET_ERROR;
  }
  if (ret_val > 0) {
    socklen_t out_size = sizeof(from_addr);

    auto size = recvfrom(socket_, static_cast<char*>(buff), max_size, 0, reinterpret_cast<sockaddr*>(&from_addr), &out_size);

    if (size == SOCKET_ERROR) {
      std::cout << "UDPServer::Receive error message - " << get_error_string(get_last_error()) << std::endl;
    }
    return size;
  }
  return SOCKET_TIMEOUT;
}

std::string GetHostName() {
  char host_name[network::kHostNameMax + 1];

  if (gethostname(host_name, sizeof(host_name)) < 0) {
    std::cout << "Failed to retrieve host name" << std::endl;
  }

  return host_name;
}

#if defined(_WIN64)

void Startup() {
  WSADATA wsaData;

  auto error_code = WSAStartup(MAKEWORD(2, 2), &wsaData);

  if (error_code != 0) {
    std::cout << "WSAStartup failed with error: " + get_error_string(error_code) << std::endl;
    Exit();
  }
}

void Cleanup() { WSACleanup(); }

#else

void Startup() {}

void Cleanup() {}

#endif

}  // namespace network


#if !defined(_WIN64)

uint64_t htonll(uint64_t value) {
  if (__BYTE_ORDER == __BIG_ENDIAN) {
    return (value);
  } else {
    uint32_t u = htonl(value >> 32);
    uint32_t l = htonl(value);

    return ((uint64_t(u) << 32) | l);
  }
}

uint64_t ntohll(uint64_t value) {
  if (__BYTE_ORDER == __BIG_ENDIAN) {
    return (value);
  } else {
    uint32_t u = ntohl(value >> 32);
    uint32_t l = ntohl(value);

    return ((uint64_t(u) << 32) | l);
  }
}

#endif
