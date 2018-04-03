#pragma once

#include <string>
#include <limits.h>

namespace network {

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

enum Request : uint16_t { Empty, Join, Leave, Start, Stop, Progress, HeartBeat };

inline std::string ToString(Request request) {
  switch (request) {
    case Empty:
      return "Request::Empty";
    case Join:
      return "Request::Join";
    case Leave:
      return "Request::Leave";
    case Start:
      return "Request::Start";
    case Stop:
      return "Request::Stop";
    case Progress:
      return "Request::Progress";
    case HeartBeat:
      return "Request::HeartBeat";
  }
  return "";
}

enum GameState : uint16_t { None, Idle, Playing, GameOver };

inline std::string ToString(GameState state) {
  switch (state) {
    case None:
      return "GameState::None";
    case Idle:
      return "GameState::Idle";
    case Playing:
      return "GameState::Playing";
    case GameOver:
      return "GameState::GameOver";
  }
  return "";
}

class Header {
 public:
  Header() :  sequence_nr_(htonl(0)), request_(static_cast<Request>(htons(Request::Empty))) { host_name_[0] = '\0'; }

  Header(const std::string& name, Request request, size_t sequence_nr) :
      sequence_nr_(htonl(sequence_nr)), request_(static_cast<Request>(htons(request))) {
    std::copy(std::begin(name), std::end(name), host_name_);
  }

  std::string host_name() const { return host_name_; }

  Request request() const { return static_cast<Request>(ntohs(request_)); }

  uint32_t sequence_nr() const { return ntohl(sequence_nr_); }

  bool operator==(const Header& header) const { return header.host_name_ == host_name_; }

  bool operator==(const std::string& host_name) const { return host_name == host_name_; }

  bool operator==(Request r) const { return r == request(); }

 private:
  char host_name_[_POSIX_HOST_NAME_MAX + 1];
  uint32_t sequence_nr_;
  Request request_;
};

class Payload {
 public:
  Payload(uint16_t lines, uint32_t score, uint16_t level, GameState state) {
    lines_ = htons(lines);
    score_ = htonl(score);
    level_ = htons(level);
    state_ = static_cast<GameState>(htons(state));
  }

  uint16_t lines() const { return htons(lines_); }

  uint32_t score() const { return htonl(score_); }

  uint16_t level() const { return htons(level_); }

  GameState state() const { return static_cast<GameState>(htons(state_)); }

 private:
  uint16_t lines_;
  uint32_t score_;
  uint16_t level_;
  GameState state_;
};

struct Package {
  Header header_;
  Payload payload_;
};

#pragma pack(pop)

} // namespace network
