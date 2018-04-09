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

const int kMaxPlayers = 9;
const int kHostNameMax = 31;
const uint32_t kID = 0x50415243; // PARC
const int kMTU = 512;
const int kWindowSize = 10;
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

enum Request : uint8_t { Empty, Join, Leave, NewGame, ResetCountDown, StartGame, ProgressUpdate, HeartBeat };

inline std::string ToString(Request request) {
  switch (request) {
    case Empty:
      return "Request::Empty";
    case Join:
      return "Request::Join";
    case Leave:
      return "Request::Leave";
    case NewGame:
      return "Request::NewGame";
    case ResetCountDown:
      return "Request::ResetCountDown";
    case StartGame:
      return "Request::StartGame";
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
      return "None";
    case Idle:
      return "Idle";
    case Waiting:
      return "Waiting";
    case Playing:
      return "Playing";
    case GameOver:
      return "Game Over";
  }
  return "";
}

class Header final {
 public:
  Header() : id_(htonl(kID)) { host_name_[0] = '\0'; }

  std::string host_name() const { return host_name_; }

  void SetHostName(const std::string& name) {
    std::copy(std::begin(name), std::end(name), host_name_);
    host_name_[name.size()] = '\0';
  }

  bool VerifyHeader() const { return htonl(kID) == id_; }

  bool operator==(const Header& header) const { return header.host_name_ == host_name_; }

  bool operator==(const std::string& host_name) const { return host_name == host_name_; }

 private:
  uint32_t id_;
  char host_name_[kHostNameMax + 1];
};

class PackageHeader final {
 public:
  PackageHeader() : id_(htonl(kID)), sequence_nr_(htonl(0)), request_(static_cast<Request>(htons(Request::Empty))) {}

  PackageHeader(Request request) : id_(htonl(kID)), sequence_nr_(htonl(0)), request_(request) {}

  PackageHeader(Request request, uint32_t sequence_nr) : id_(htonl(kID)), sequence_nr_(htonl(sequence_nr)), request_(request) {}

  bool VerifyHeader() const { return htonl(kID) == id_; }

  uint32_t sequence_nr() const { return ntohl(sequence_nr_); }

  void SetSeqenceNr(uint32_t n) { sequence_nr_ = htonl(n); }

  Request request() const { return request_; }

  void SetRequest(Request r) { request_ = r; }

  bool operator==(Request r) const { return r == request(); }

 private:
  uint32_t id_;
  uint32_t sequence_nr_;
  Request request_;
};

class Payload final {
 public:
  Payload() : lines_(0), score_(0), level_(0), garbage_(0), state_(GameState::Idle) {}

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

  void SetState(GameState state) { state_ = state; }

 private:
  uint16_t lines_;
  uint32_t score_;
  uint8_t level_;
  uint8_t garbage_;
  GameState state_;
};

struct Package {
  PackageHeader header_;
  Payload payload_;
};

struct Packages {
  Packages() : size_(0) {}

  Packages(const std::string& host_name, uint8_t size) : size_(size) {
    static_assert(sizeof(Packages) <= kMTU);
    header_.SetHostName(host_name);
  }

  int64_t size() const { return size_; }

  Header header_;
  Package array_[kWindowSize];
  uint8_t size_;
};

inline Package CreatePackage(Request request, GameState state = GameState::None) {
  Package package;

  package.header_ = PackageHeader(request);
  package.payload_.SetState(state);

  return package;
}

#pragma pack(pop)

} // namespace network
