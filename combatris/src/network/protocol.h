#pragma once

#include <string>
#include <limits.h>

namespace network {

const std::string kDefaultServer = "0.0.0.0";
const int kDefaultPort = 3001;

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
  Header() : request_(Request::Empty), sequence_nr_(0) {
    host_name_[0] = '\0';
  }
  Header(const std::string& name, Request request, uint64_t sequence_nr) :
      request_(request), sequence_nr_(sequence_nr) {
    std::copy(std::begin(name), std::end(name), host_name_);
  }
  char host_name_[_POSIX_HOST_NAME_MAX + 1];
  Request request_;
  size_t sequence_nr_;
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
