#include <gtest/gtest.h>

#include "moonlight/scoreboard.h"
#include "moonlight/synthetic.h"

using namespace moonlight;

namespace {
TraceInst op(KernelTrace& k, std::initializer_list<RegId> dst, std::initializer_list<RegId> src) {
  return synthetic::make_inst(k, "IADD3", 0, dst, src);
}
}  // namespace

TEST(Scoreboard, FreshScoreboardAllowsEverything) {
  KernelTrace k;
  Scoreboard sb(4);
  EXPECT_TRUE(sb.can_issue(0, op(k, {1}, {2, 3})));
}

TEST(Scoreboard, DISABLED_RawHazardBlocksUntilRelease) {
  KernelTrace k;
  Scoreboard sb(4);
  sb.reserve(0, op(k, {5}, {}));
  EXPECT_FALSE(sb.can_issue(0, op(k, {6}, {5})));  // reads R5
  sb.release(0, 5);
  EXPECT_TRUE(sb.can_issue(0, op(k, {6}, {5})));
}

TEST(Scoreboard, DISABLED_WawHazardBlocks) {
  KernelTrace k;
  Scoreboard sb(4);
  sb.reserve(0, op(k, {5}, {}));
  EXPECT_FALSE(sb.can_issue(0, op(k, {5}, {})));
}

TEST(Scoreboard, DISABLED_WarpsAreIndependent) {
  KernelTrace k;
  Scoreboard sb(4);
  sb.reserve(0, op(k, {5}, {}));
  EXPECT_TRUE(sb.can_issue(1, op(k, {6}, {5})));
}

TEST(Scoreboard, DISABLED_NoRegIsNeverADependency) {
  KernelTrace k;
  Scoreboard sb(4);
  sb.reserve(0, op(k, {kNoReg}, {}));
  EXPECT_TRUE(sb.can_issue(0, op(k, {1}, {kNoReg})));
}

TEST(Scoreboard, DISABLED_ResetClearsPendingRegisters) {
  KernelTrace k;
  Scoreboard sb(4);
  sb.reserve(0, op(k, {5}, {}));
  sb.reset(0);
  EXPECT_TRUE(sb.can_issue(0, op(k, {6}, {5})));
}
