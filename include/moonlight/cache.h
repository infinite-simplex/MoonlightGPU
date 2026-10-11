#pragma once

#include <cstdint>

#include "moonlight/config.h"

namespace moonlight {

enum class AccessOutcome { Hit, Miss, ReservationFail };

struct CacheStats {
  std::uint64_t hits = 0;
  std::uint64_t misses = 0;
  std::uint64_t reservation_fails = 0;
};

// Set-associative, sectored cache (tags only; no data).
class SetAssocCache {
 public:
  explicit SetAssocCache(const CacheConfig& cfg);

  AccessOutcome access(Addr addr, bool is_write, Cycle now);

  const CacheStats& stats() const { return stats_; }
  const CacheConfig& config() const { return cfg_; }

 private:
  CacheConfig cfg_;
  CacheStats stats_;
  // TODO(week 3): sets x ways of {tag, sector valid bits, LRU state}.
};

}  // namespace moonlight
