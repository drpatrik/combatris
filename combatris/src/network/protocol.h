#pragma once

#if defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#pragma warning(disable:4996) // _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4267) // conversion from size_t to int
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include <string>
#include <limits.h>

namespace network {

const int kHostNameMax = 25;
const std::string kEnvServer = "COMBATRIS_BROADCAST_IP";
const std::string kEnvPort = "COMBATRIS_BROADCAST_PORT";

const std::string kDefaultBroadcastIP = "192.168.1.255";
const int kDefaultPort = 11000;

inline std::string GetBroadcastIP() {
  auto env = getenv(kEnvServer.c_str());

  if (nullptr == env) {
    return kDefaultBroadcastIP;
  }
  return env;
}

inline int GetPort() {
  auto env = getenv(kEnvPort.c_str());

  if (nullptr == env) {
    return kDefaultPort;
  }
  return std::stoi(env);
}

#pragma pack(push, 1)

enum Request : uint8_t { Empty, Join, Leave, ResetCounter, StartGame, ProgressUpdate, HeartBeat };

inline std::string ToString(Request request) {
  switch (request) {
    case Empty:
      return "Request::Empty";
    case Join:
      return "Request::Join";
    case Leave:
      return "Request::Leave";
    case ResetCounter:
      return "Request::ResetCounter";
    case StartGame:
      return "Request::HeartBeat";
    case ProgressUpdate:
      return "Request::ProgressUpdate";
    case HeartBeat:
      return "Request::Heartbeat";
  }
  return "";
}

enum GameState : uint8_t { None, Idle, Waiting, Playing, GameOver };

inline std::string ToString(GameState state) {
  switch (state) {
    case None:
      return "GameState::None";
    case Idle:
      return "GameState::Idle";
    case Waiting:
      return "GameState::Waiting";
    case Playing:
      return "GameState::Playing";
    case GameOver:
      return "GameState::GameOver";
  }
  return "";
}

class Header final {
 public:
  Header() : sequence_nr_(htonl(0)), request_(static_cast<Request>(htons(Request::Empty))) { host_name_[0] = '\0'; }

  Header(Request r) : sequence_nr_(htonl(0)), request_(r) { host_name_[0] = '\0'; }

  Header(const std::string& name, Request request, uint32_t sequence_nr) :
      sequence_nr_(htonl(sequence_nr)), request_(request) {
    set_host_name(name);
  }

  std::string host_name() const { return host_name_; }

  void set_host_name(const std::string& name) {
    std::copy(std::begin(name), std::end(name), host_name_);
    host_name_[name.size()] = '\0';
  }

  Request request() const { return request_; }

  uint32_t sequence_nr() const { return ntohl(sequence_nr_); }

  void set_seqence_nr(uint32_t n) { sequence_nr_ = htonl(n); }

  bool operator==(const Header& header) const { return header.host_name_ == host_name_; }

  bool operator==(const std::string& host_name) const { return host_name == host_name_; }

  bool operator==(Request r) const { return r == request(); }

 private:
  uint32_t sequence_nr_;
  Request request_;
  char host_name_[kHostNameMax + 1];
};

class Payload final {
 public:
  Payload() : lines_(0), score_(0), level_(0), garbage_(0), state_(GameState::None) {}

  Payload(uint16_t lines, uint32_t score, uint8_t level, uint8_t garbage, GameState state) {
    lines_ = htons(lines);
    score_ = htonl(score);
    level_ = level;
    garbage_ = garbage;
    state_ = state;
  }

  uint16_t lines() const { return htons(lines_); }

  uint32_t score() const { return htonl(score_); }

  uint8_t level() const { return level_; }

  uint8_t garbage() const { return garbage_; }

  GameState state() const { return state_; }

 private:
  uint16_t lines_;
  uint32_t score_;
  uint8_t level_;
  uint8_t garbage_;
  GameState state_;
};

struct Package {
 public:
  Header header_;
  Payload payload_;
};

#pragma pack(pop)

} // namespace network
