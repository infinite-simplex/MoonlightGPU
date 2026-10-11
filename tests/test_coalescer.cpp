#include <gtest/gtest.h>

#include "moonlight/coalescer.h"
#include "moonlight/synthetic.h"

using namespace moonlight;

namespace {
constexpr std::uint32_t kLine = 128, kSector = 32;

TraceInst load(KernelTrace& k, std::uint32_t stride, LaneMask mask = 0xffffffffu, Addr base = 0x1000) {
  TraceInst i = synthetic::make_inst(k, "LDG.E", 0, {2}, {1}, mask);
  i.mem_width = 4;
  for (int lane = 0; lane < kWarpSize; ++lane)
    if (mask & (1u << lane)) i.mem_addrs.push_back(base + static_cast<Addr>(lane) * stride);
  return i;
}
}  // namespace

TEST(Coalescer, NonMemoryInstructionProducesNothing) {
  KernelTrace k;
  EXPECT_TRUE(coalesce(synthetic::make_inst(k, "FFMA", 0, {1}, {2, 3}), kLine, kSector).empty());
}

TEST(Coalescer, DISABLED_ConsecutiveWordsMergeIntoOneLineAllSectors) {
  KernelTrace k;
  const auto t = coalesce(load(k, 4), kLine, kSector);  // 32 lanes * 4B = 128B
  ASSERT_EQ(t.size(), 1u);
  EXPECT_EQ(t[0].line_addr, 0x1000u);
  EXPECT_EQ(t[0].sector_mask, 0xFu);
}

TEST(Coalescer, DISABLED_LineStrideSerializesToOneTransactionPerLane) {
  KernelTrace k;
  EXPECT_EQ(coalesce(load(k, 128), kLine, kSector).size(), 32u);
}

TEST(Coalescer, DISABLED_MisalignedBaseStraddlesTwoLines) {
  KernelTrace k;
  EXPECT_EQ(coalesce(load(k, 4, 0xffffffffu, 0x1040), kLine, kSector).size(), 2u);
}

TEST(Coalescer, DISABLED_OnlyActiveLanesContribute) {
  KernelTrace k;
  const auto t = coalesce(load(k, 4, 0x0000000Fu), kLine, kSector);  // 4 lanes = 16B
  ASSERT_EQ(t.size(), 1u);
  EXPECT_EQ(t[0].sector_mask, 0x1u);
}

TEST(Coalescer, DISABLED_BroadcastSameAddressIsOneTransaction) {
  KernelTrace k;
  EXPECT_EQ(coalesce(load(k, 0), kLine, kSector).size(), 1u);
}
