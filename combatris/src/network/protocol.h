#pragma once

#if defined(_WIN64)

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#pragma warning(disable:4996) // _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4267) // conversion from size_t to int

#include <winsock2.h>
#include <iterator>

#else

#include <arpa/inet.h>

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

#endif

#include <string>
#include <array>
#include <iostream>
#include <functional>
#include <algorithm>
#include <limits.h>

namespace network {

const size_t kHostNameMax = 31;
const uint32_t kSignature = 0x50415243; // PARC
// UDP Maximum Transmision Unit 1500 bytes - 20 byte (IPv4 header) - 8 byte UDP-header
const int kMTU = 1472;
const int kWindowSize = 14;
const int kMatrixStateSize = 20 * 5;
using MatrixState = std::array<uint8_t, kMatrixStateSize>;

inline uint64_t SetHostName(const std::string& from, char *to) {
  auto tmp(from);

  tmp.erase(std::min(kHostNameMax, tmp.size()), std::string::npos);
#if defined(_WIN64)
  std::copy(std::begin(tmp), std::end(tmp), stdext::checked_array_iterator<char*>(to, kHostNameMax));
#else
  std::copy(std::begin(tmp), std::end(tmp), to);
#endif

  to[tmp.size()] = '\0';

  return std::hash<std::string>{}(from);
}

enum class CampaignType { None, Combatris, Marathon, Sprint, Ultra, Battle };

constexpr int ToInt(CampaignType type) { return static_cast<int>(type); }

inline std::string ToString(CampaignType type) {
  switch (type) {
    case CampaignType::None:
      return "-";
    case CampaignType::Combatris:
      return "Combatris";
    case CampaignType::Marathon:
      return "Marathon";
    case CampaignType::Sprint:
      return "Sprint";
    case CampaignType::Ultra:
      return "Ultra";
    case CampaignType::Battle:
      return "Battle";
  }
  return "Unknown";
}

enum Request : uint8_t { Empty, Join, Leave, NewGame, StartGame, NewState, SendLines, KnockedOutBy, Time, ProgressUpdate, HeartBeat };

inline std::string ToString(Request request) {
  switch (request) {
    case Request::Empty:
      return "Request::Empty";
    case Request::Join:
      return "Request::Join";
    case Request::Leave:
      return "Request::Leave";
    case Request::NewGame:
      return "Request::NewGame";
    case Request::StartGame:
      return "Request::StartGame";
    case Request::NewState:
      return "Request::NewState";
    case Request::SendLines:
      return "Request::SendLines";
    case Request::KnockedOutBy:
      return "Request::KnockedOutBy";
    case Request::Time:
      return "Request::Time";
    case Request::ProgressUpdate:
      return "Request::ProgressUpdate";
    case Request::HeartBeat:
      return "Request::Heartbeat";
  }
  return "Unknown";
}

enum class GameState : uint8_t { None, Idle, Waiting, Playing, GameOver };

inline std::string ToString(GameState state) {
  switch (state) {
    case GameState::None:
      return "None";
    case GameState::Idle:
      return "Idle";
    case GameState::Waiting:
      return "Waiting";
    case GameState::Playing:
      return "Playing";
    case GameState::GameOver:
      return "Game Over";
  }
  return "Unknown";
}

enum class Channel : uint8_t { None, Unreliable, Reliable };

#pragma pack(push, 1)

class Header final {
 public:
  Header() : signature_(htonl(kSignature)), sequence_nr_(htonl(0)), request_(static_cast<Request>(htons(Request::Empty))) {}

  Header(Request request) : signature_(htonl(kSignature)), sequence_nr_(htonl(0)), request_(request) {}

  Header(Request request, uint32_t sequence_nr) : signature_(htonl(kSignature)), sequence_nr_(htonl(sequence_nr)), request_(request) {}

  bool Verify() const { return htonl(kSignature) == signature_; }

  uint32_t sequence_nr() const { return ntohl(sequence_nr_); }

  void SetSeqenceNr(uint32_t n) { sequence_nr_ = htonl(n); }

  Request request() const { return request_; }

  void SetRequest(Request r) { request_ = r; }

  bool operator==(Request r) const { return r == request(); }

 private:
  uint32_t signature_;
  uint32_t sequence_nr_;
  Request request_;
};

class ProgressPayload final {
 public:
  ProgressPayload() : score_(0), lines_(0), level_(0) {}

  ProgressPayload(uint16_t lines, uint32_t score, uint8_t level) {
    lines_ = htons(lines);
    score_ = htonl(score);
    level_ = level;
  }

  ProgressPayload(uint16_t lines, uint32_t score, uint8_t level, const MatrixState& matrix_state) {
    lines_ = htons(lines);
    score_ = htonl(score);
    level_ = level;
    std::copy(matrix_state.begin(), matrix_state.end(), matrix_state_);
  }

  inline uint16_t lines() const { return ntohs(lines_); }

  inline uint32_t score() const { return ntohl(score_); }

  inline uint8_t level() const { return level_; }

  MatrixState matrix_state() const {
    MatrixState matrix_state;

    static_assert(sizeof(MatrixState::value_type) == 1);
    std::copy(matrix_state_, matrix_state_ + kMatrixStateSize, matrix_state.begin());

    return matrix_state;
  }

 private:
  uint32_t score_;
  uint16_t lines_;
  uint8_t level_;
  MatrixState::value_type matrix_state_[kMatrixStateSize];
};

class Payload final {
 public:
  Payload() : value_(0), state_(GameState::None) {}

  explicit Payload(GameState state) : state_(state) {}

  explicit Payload(uint64_t value) { SetValue(value); }

  inline uint64_t value() const { return ntohll(value_); }

  inline void SetValue(uint64_t value) { value_ = htonll(value); }

  inline GameState state() const { return state_; }

  inline void SetState(GameState state) { state_ = state; }

 private:
  uint64_t value_ = 0;
  GameState state_ = GameState::None;
};

class PackageHeader final {
 public:
  PackageHeader() : signature_(htonl(kSignature)), host_id_(0), channel_(Channel::None) {
    host_name_[0] = '\0';
  }

  explicit PackageHeader(Channel channel) : signature_(htonl(kSignature)), host_id_(0), channel_(channel) {
    host_name_[0] = '\0';
  }

  inline std::string host_name() const { return host_name_; }

  inline void SetHostName(const std::string& name) { host_id_ = network::SetHostName(name, host_name_); }

  inline int64_t host_id() const  { return host_id_; }

  inline bool Verify() const { return htonl(kSignature) == signature_; }

  inline Channel channel() const { return channel_; }

 private:
  uint32_t signature_;
  char host_name_[kHostNameMax + 1];
  uint64_t host_id_;
  Channel channel_;
};

struct ProgressPackage {
  Header header_;
  ProgressPayload payload_;
};

struct UnreliablePackage {
  UnreliablePackage(const std::string& host_name, const ProgressPackage& package) : package_(package) {
    static_assert(sizeof(UnreliablePackage) <= kMTU);
    header_.SetHostName(host_name);
  }

  PackageHeader header_ = PackageHeader(Channel::Unreliable);
  ProgressPackage package_;
};

struct Package {
  Header header_;
  Payload payload_;
};

struct PackageArray {
  PackageArray() : size_(0) {}

  PackageArray(uint8_t size) : size_(size) {}

  int size() const { return size_; }

  Package packages_[kWindowSize];
  uint8_t size_;
};

struct ReliablePackage {
  ReliablePackage() : package_(0) {}

  ReliablePackage(const std::string& host_name, uint8_t size) : package_(size) {
    static_assert(sizeof(ReliablePackage) <= kMTU);
    header_.SetHostName(host_name);
  }

  inline int size() const { return package_.size_; }

  inline bool Verify() const { return header_.Verify(); }

  PackageHeader header_ = PackageHeader(Channel::Reliable);
  PackageArray package_;
};

inline auto CreatePackage(Request request) {
  Package package;

  package.header_ = Header(request);

  return package;
}

inline auto CreatePackage(Request request, GameState state) {
  Package package;

  package.header_ = Header(request);
  package.payload_.SetState(state);

  return package;
}

inline auto CreatePackage(CampaignType type) {
  Package package;

  package.header_ = Header(Request::NewGame);
  package.payload_.SetState(GameState::Waiting);
  package.payload_.SetValue(ToInt(type));

  return package;
}

inline auto CreatePackage(Request request, uint64_t value) {
  Package package;

  package.header_ = Header(request);
  package.payload_.SetValue(value);

  return package;
}

inline auto CreatePackage(uint16_t lines, uint32_t score, uint8_t level) {
  ProgressPackage package;

  package.header_ = Header(Request::ProgressUpdate);
  package.payload_ = ProgressPayload(lines, score, level);

  return package;
}

inline auto CreatePackage(uint16_t lines, uint32_t score, uint8_t level, const MatrixState& state) {
  ProgressPackage package;

  package.header_ = Header(Request::ProgressUpdate);
  package.payload_ = ProgressPayload(lines, score, level, state);

  return package;
}

#pragma pack(pop)

 } // namespace network
