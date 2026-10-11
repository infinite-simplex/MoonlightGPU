#include <gtest/gtest.h>

#include "moonlight/scheduler.h"

using namespace moonlight;

namespace {

std::vector<Warp> make_warps(int n) {
  std::vector<Warp> w(static_cast<std::size_t>(n));
  for (int i = 0; i < n; ++i) {
    w[static_cast<std::size_t>(i)].valid = true;
    w[static_cast<std::size_t>(i)].slot = i;
    w[static_cast<std::size_t>(i)].age = static_cast<std::uint64_t>(i);
  }
  return w;
}

}  // namespace

TEST(Scheduler, ReturnsMinusOneWhenNothingIsReady) {
  auto s = make_scheduler(SchedulerPolicy::RoundRobin);
  const auto warps = make_warps(4);
  EXPECT_EQ(s->select(warps, {0, 0, 0, 0}, 0), -1);
}

TEST(Scheduler, DISABLED_RoundRobinRotatesThroughReadyWarps) {
  auto s = make_scheduler(SchedulerPolicy::RoundRobin);
  const auto warps = make_warps(4);
  const std::vector<std::uint8_t> ready{1, 1, 1, 1};
  for (int expect : {0, 1, 2, 3, 0, 1}) {
    const int got = s->select(warps, ready, 0);
    EXPECT_EQ(got, expect);
    s->on_issue(got, 0);
  }
}

TEST(Scheduler, DISABLED_RoundRobinSkipsStalledWarps) {
  auto s = make_scheduler(SchedulerPolicy::RoundRobin);
  const auto warps = make_warps(4);
  const std::vector<std::uint8_t> ready{1, 0, 1, 0};
  int got = s->select(warps, ready, 0);
  s->on_issue(got, 0);
  EXPECT_EQ(got, 0);
  EXPECT_EQ(s->select(warps, ready, 1), 2);
}

TEST(Scheduler, DISABLED_GtoSticksWithLastIssuedWhileReady) {
  auto s = make_scheduler(SchedulerPolicy::GreedyThenOldest);
  const auto warps = make_warps(4);
  s->on_issue(2, 0);
  EXPECT_EQ(s->select(warps, {1, 1, 1, 1}, 1), 2);
}

TEST(Scheduler, DISABLED_GtoFallsBackToOldestWhenGreedyWarpStalls) {
  auto s = make_scheduler(SchedulerPolicy::GreedyThenOldest);
  const auto warps = make_warps(4);  // slot i has age i; smaller age == older
  s->on_issue(2, 0);
  EXPECT_EQ(s->select(warps, {0, 1, 0, 1}, 1), 1);
}
