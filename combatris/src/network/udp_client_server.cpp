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

#endif

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>

#include <iostream>

namespace {

const std::string kEnvServer = "COMBATRIS_BROADCAST_IP";
const std::string kEnvPort = "COMBATRIS_BROADCAST_PORT";
const std::string kDefaultBroadcastIP = "192.168.1.255";
const int kDefaultPort = 11000;

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

ULONG& GetAddressAsUnsigned(in_addr& addr) { return addr.S_un.S_addr; }

#define close closesocket

#else

int get_last_error() { return errno; }

std::string get_error_string(int error_code) { return strerror(error_code); }

unsigned& GetAddressAsUnsigned(in_addr& addr) { return addr.s_addr; }

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

bool IsValidAddress(unsigned ip) {
  auto c = (ip >> 24) & 0xFF;

  return (c != 169 && c != 127);
}

// Handles IP4 addresses only
std::string FindBroadcastAddress() {
  addrinfo hints{};

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;

  addrinfo* addrs = nullptr;

  auto ret_val = getaddrinfo(network::GetHostName().c_str(), nullptr, &hints, &addrs);

  if (ret_val != 0) {
    std::cout << "getaddrinfo failed with error: " << get_error_string(ret_val) << std::endl;
    return kDefaultBroadcastIP;
  }
  auto address = kDefaultBroadcastIP;

  for (auto addr = addrs; addr != nullptr; addr = addr->ai_next) {
    if (AF_INET == addrs->ai_family) {
      auto sockaddr_ipv4 = reinterpret_cast<sockaddr_in*>(addr->ai_addr);
      auto ip = ntohl(GetAddressAsUnsigned(sockaddr_ipv4->sin_addr));

      if (IsValidAddress(ip)) {
        if (address != kDefaultBroadcastIP) {
          std::cout << "Warning - several network interfaces found" << std::endl;
          break;
        }
        GetAddressAsUnsigned(sockaddr_ipv4->sin_addr) = htonl(ip | 0xFF);
        address = inet_ntoa(sockaddr_ipv4->sin_addr);
      }
    }
  }
  if (addrs != nullptr) {
    freeaddrinfo(addrs);
  }

  return address;
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
  const auto ret_val(select(socket_ + 1, &fds, nullptr, nullptr, &timeout));

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
  const auto ret_val(select(socket_ + 1, &fds, nullptr, nullptr, &timeout));

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

std::string GetBroadcastAddress() {
  auto env = getenv(kEnvServer.c_str());

  if (nullptr == env) {
    return FindBroadcastAddress();
  }
  return env;
}

int GetPort() {
  auto env = getenv(kEnvPort.c_str());

  if (nullptr == env) {
    return kDefaultPort;
  }
  return std::stoi(env);
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
