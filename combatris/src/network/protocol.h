#pragma once

#include <string>
#include <limits.h>

namespace network {

const std::string kEnvServer = "COMBATRIS_BROADCAST_IP";
const std::string kEnvPort = "COMBATRIS_BROADCAST_PORT";

const std::string kDefaultServer = "192.168.1.255";
const int kDefaultPort = 3001;

inline std::string GetServer() {
  auto env = getenv(kEnvServer.c_str());

  if (nullptr == env) {
    return kDefaultServer;
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

enum Request { Empty, Join, Leave, Start, Stop, Progress, HeartBeat };

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

enum GameState { None, Idle, Playing, GameOver };

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

struct Header {
  Header() :  sequence_nr_(0), request_(Request::Empty) {
    host_name_[0] = '\0';
  }
  Header(const std::string& name, Request request, size_t sequence_nr) :
      sequence_nr_(sequence_nr), request_(request) {
    std::copy(std::begin(name), std::end(name), host_name_);
  }
  char host_name_[_POSIX_HOST_NAME_MAX + 1];
  size_t sequence_nr_;
  Request request_;
};

struct Payload {
  int lines_ = 0;
  int score_ = 0;
  int level_ = 0;
  GameState state_ = GameState::None;
};

struct Package {
  Header header_;
  Payload payload_;
};

#pragma pack(pop)

} // namespace network
