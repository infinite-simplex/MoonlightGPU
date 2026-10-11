#include "moonlight/cache.h"

namespace moonlight {

SetAssocCache::SetAssocCache(const CacheConfig& cfg) : cfg_(cfg) {}

AccessOutcome SetAssocCache::access(Addr /*addr*/, bool /*is_write*/, Cycle /*now*/) {
  // TODO(week 3): index/tag lookup, sector valid bits, LRU update and allocation.
  ++stats_.misses;
  return AccessOutcome::Miss;
}

}  // namespace moonlight
