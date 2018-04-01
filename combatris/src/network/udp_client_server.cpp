#include "network/udp_client_server.h"

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

#include <iostream>

namespace {

std::string GetHostName() {
  char host_name[_POSIX_HOST_NAME_MAX + 1];

  if (gethostname(host_name, sizeof(host_name)) < 0) {
    std::cout << "Failed to retrieve host name" << std::endl;
  }

  return host_name;
}

} // namespace

namespace network {

Client::Client(const std::string& addr, int port) {
  const auto port_str(std::to_string(port));

  addrinfo hints{};

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;

  const auto ret_value(getaddrinfo(addr.c_str(), port_str.c_str(), &hints, &addr_info_));

  if (ret_value != 0 || addr_info_ == nullptr) {
    std::cout << "Client: " << "invalid address or port: \"" << addr << ":" << port << "\"" << std::endl;
    std::cout << "Client: error message: " << strerror(errno) << std::endl;
    exit(-1);
  }
  socket_ = socket(addr_info_->ai_family, SOCK_DGRAM, IPPROTO_UDP);

  if (socket_ < 0) {
    std::cout << "Client: could not create socket for:  \"" << addr << ":" << port << "\"" << std::endl;
    std::cout << "Client: error message: " << strerror(errno) << std::endl;
    exit(-1);
  }
  if (fcntl(socket_, F_SETFD, FD_CLOEXEC) < 0) {
    std::cout << "Client: fcntl failed: " << errno << std::endl;
    std::cout << "Client: error message: " << strerror(errno) << std::endl;
    exit(-1);
  }
  host_name_ = GetHostName();
}

Client::~Client() noexcept {
  if (addr_info_ != nullptr) {
    freeaddrinfo(addr_info_);
  }
  if (socket_ != -1) {
    close(socket_);
  }
}

ssize_t Client::Send(void* buff, size_t size) {
  auto ret_value = sendto(socket_, buff, size, 0, addr_info_->ai_addr, addr_info_->ai_addrlen);

  if (ret_value == -1) {
    std::cout << "Client::Send error message: " << strerror(errno) << std::endl;
  }

  return ret_value;
}

Server::Server(const std::string& addr, int port) {
  addrinfo hints{};

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;

  auto ret_value(getaddrinfo(addr.c_str(), std::to_string(port).c_str(), &hints, &addr_info_));

  if (ret_value != 0 || addr_info_ == nullptr) {
    std::cout << "Server: " << "invalid address or port: \"" << addr << ":" << port << "\"" << std::endl;
    std::cout << "Server: error message: " << strerror(errno) << std::endl;
    exit(-1);
  }
  socket_ = socket(addr_info_->ai_family, SOCK_DGRAM, IPPROTO_UDP);

  if (socket_ < 0) {
    std::cout << "Server: could not create socket for:  \"" << addr << ":" << port << "\"" << std::endl;
    std::cout << "Server: error message: " << strerror(errno) << std::endl;
    exit(-1);
  }
  if (fcntl(socket_, F_SETFD, FD_CLOEXEC) < 0) {
    std::cout << "Server: fcntl failed: " << errno << std::endl;
    std::cout << "Server: error message: " << strerror(errno) << std::endl;
    exit(-1);
  }
  ret_value = bind(socket_, addr_info_->ai_addr, addr_info_->ai_addrlen);

  if (ret_value != 0) {
    std::cout << "Server: could not bind socket with:  \"" << addr << ":" << port << "\" " << std::endl;
    std::cout << "Server: error message: " << strerror(errno) << std::endl;
  }
}

Server::~Server() noexcept {
  if (addr_info_ != nullptr) {
    freeaddrinfo(addr_info_);
  }
  if (socket_ != -1) {
    close(socket_);
  }
}

ssize_t Server::Receive(void* buff, size_t max_size, int max_wait_ms) {
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(socket_, &fds);

  timeval timeout{};
  timeout.tv_sec = max_wait_ms / 1000;
  timeout.tv_usec = (max_wait_ms % 1000) * 1000;
  const auto ret_val(select(socket_ + 1, &fds, nullptr, &fds, &timeout));

  if (ret_val == -1) {
    std::cout << "Server::Receive error message: " << strerror(errno) << std::endl;
    return -1;
  }
  if (ret_val > 0) {
    return recv(socket_, buff, max_size, 0);
  }
  errno = EAGAIN;
  return -1;
}

}  // namespace network
