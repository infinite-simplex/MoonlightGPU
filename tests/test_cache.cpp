#include <gtest/gtest.h>

#include "moonlight/cache.h"

using namespace moonlight;

namespace {
CacheConfig tiny() {
  CacheConfig c;
  c.size_bytes = 2 * 128 * 2;  // 2 sets x 2 ways x 128B lines
  c.line_bytes = 128;
  c.sector_bytes = 32;
  c.assoc = 2;
  return c;
}
}  // namespace

TEST(Cache, FirstAccessMisses) {
  SetAssocCache c(tiny());
  EXPECT_EQ(c.access(0x0, false, 0), AccessOutcome::Miss);
}

TEST(Cache, DISABLED_SecondAccessToSameLineHits) {
  SetAssocCache c(tiny());
  c.access(0x0, false, 0);
  EXPECT_EQ(c.access(0x4, false, 1), AccessOutcome::Hit);
}

TEST(Cache, DISABLED_LruEvictsLeastRecentlyUsedWay) {
  SetAssocCache c(tiny());  // set index = (addr / 128) % 2, so 0x000/0x100/0x200 share set 0
  c.access(0x000, false, 0);
  c.access(0x100, false, 1);
  c.access(0x000, false, 2);  // 0x100 is now LRU
  c.access(0x200, false, 3);  // evicts 0x100
  EXPECT_EQ(c.access(0x000, false, 4), AccessOutcome::Hit);
  EXPECT_EQ(c.access(0x100, false, 5), AccessOutcome::Miss);
}

TEST(Cache, DISABLED_SectoredLineMissesOnUntouchedSector) {
  SetAssocCache c(tiny());
  c.access(0x00, false, 0);                                    // fills sector 0 only
  EXPECT_EQ(c.access(0x20, false, 1), AccessOutcome::Miss);   // sector 1 of same line
}

TEST(Cache, DISABLED_StatsTrackHitsAndMisses) {
  SetAssocCache c(tiny());
  c.access(0x0, false, 0);
  c.access(0x0, false, 1);
  EXPECT_EQ(c.stats().misses, 1u);
  EXPECT_EQ(c.stats().hits, 1u);
}
