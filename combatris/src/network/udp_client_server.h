#pragma once

#include <sys/socket.h>
#include <netdb.h>

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
  int socket_ = -1;
  addrinfo* addr_info_ = nullptr;
  std::string host_name_;
};

class Server {
 public:
  Server(const Server&) = delete;

  Server(std::string const &broadcast_address, int port);

  ~Server() noexcept;

  ssize_t Receive(void* buff, size_t max_size) { return recv(socket_, buff, max_size, 0); }

  ssize_t Receive(void* buff, size_t max_size, int max_wait_ms);

 private:
  int socket_ = -1;
  addrinfo* addr_info_ = nullptr;
};

}  // namespace network
