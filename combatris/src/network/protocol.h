#pragma once

#if defined(_WIN64)

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#pragma warning(disable:4996) // _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4267) // conversion from size_t to int

#include <winsock2.h>

#else

#if !defined(__APPLE__)

#include <endian.h>

inline uint64_t htonll(uint64_t value) {
  if (__BYTE_ORDER == __BIG_ENDIAN) {
    return value;
  }
  return (static_cast<uint64_t>(htonl(value)) << 32) | htonl(value >> 32);
}

inline uint64_t ntohll(uint64_t value) {
  if (__BYTE_ORDER == __BIG_ENDIAN) {
    return value;
  }
  return (static_cast<uint64_t>(ntohl(value)) << 32) | ntohl(value >> 32);
}

#endif // !__APPLE__

#include <arpa/inet.h>

#endif

#include <string>
#include <functional>
#include <algorithm>
#include <limits.h>

namespace network {

const size_t kHostNameMax = 31;
const uint32_t kID = 0x50415243; // PARC
const int kMTU = 512;
const int kWindowSize = 9;
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

inline void SetHostName(const std::string& from, char *to) {
  auto tmp(from);

  tmp.erase(std::min(kHostNameMax, tmp.size()), std::string::npos);
  std::copy(std::begin(tmp), std::end(tmp), to);
  to[tmp.size()] = '\0';
}

enum Request : uint8_t { Empty, Join, Leave, NewGame, StartGame, ProgressUpdate, HeartBeat };

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

#pragma pack(push, 1)

class Header final {
 public:
  Header() : id_(htonl(kID)) { host_name_[0] = '\0'; }

  std::string host_name() const { return host_name_; }

  void SetHostName(const std::string& name) { network::SetHostName(name, host_name_); }

  bool VerifyHeader() const { return htonl(kID) == id_; }

  bool operator==(const Header& header) const { return header.host_name_ == host_name_; }

  bool operator==(const std::string& host_name) const { return host_name == host_name_; }

 private:
  char host_name_[kHostNameMax + 1];
  uint32_t id_;
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
  explicit Payload(GameState state = GameState::Idle)
      : knocked_out_by_(0), score_(0), lines_(0), lines_sent_(0), level_(0), lines_got_(0), state_(state) {}

  Payload(uint16_t lines, uint16_t lines_sent, uint32_t score, uint8_t level, uint8_t lines_got, GameState state)
      : knocked_out_by_(0) {
    lines_ = htons(lines);
    lines_sent_ = htons(lines_sent);
    score_ = htonl(score);
    level_ = level;
    lines_got_ = lines_got;
    state_ = state;
  }

  uint16_t lines() const { return ntohs(lines_); }

  uint16_t lines_sent() const { return ntohs(lines_sent_); }

  uint32_t score() const { return ntohl(score_); }

  uint8_t level() const { return level_; }

  uint8_t lines_got() const { return lines_got_; }

  uint64_t knocked_out_by() const { return ntohll(knocked_out_by_); }

  void SetKnockoutBy(const std::string& name) { knocked_out_by_ = htonll(std::hash<std::string>{}(name)); }

  GameState state() const { return state_; }

  void SetState(GameState state) { state_ = state; }

 private:
  uint64_t knocked_out_by_;
  uint32_t score_;
  uint16_t lines_;
  uint16_t lines_sent_;
  uint8_t level_;
  uint8_t lines_got_;
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
