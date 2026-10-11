#include <gtest/gtest.h>

#include "moonlight/simulator.h"
#include "moonlight/synthetic.h"

using namespace moonlight;

TEST(Simulator, TerminatesAndCountsAllInstructions) {
  const KernelTrace k = synthetic::alu_independent(4, 50);
  const Stats s = Simulator(SimConfig{}).run(k);
  EXPECT_EQ(s.warp_insts_issued, k.total_warp_insts());
  EXPECT_EQ(s.thread_insts, k.total_warp_insts() * 32);
  EXPECT_GT(s.cycles, 0u);
}

TEST(Simulator, EmptyKernelFinishesImmediately) {
  const KernelTrace k;
  const Stats s = Simulator(SimConfig{}).run(k);
  EXPECT_EQ(s.warp_insts_issued, 0u);
  EXPECT_EQ(s.cycles, 0u);
}

TEST(Simulator, WatchdogFiresInsteadOfHanging) {
  SimConfig cfg;
  cfg.max_cycles = 3;
  const KernelTrace k = synthetic::alu_independent(1, 100);
  EXPECT_THROW(Simulator(cfg).run(k), std::runtime_error);
}

// ---- Specs for behaviour you will implement (enable as you go) -------------------

// A single warp can issue at most one instruction per cycle, so N independent ops
// take at least N cycles (placeholder model: exactly N).
TEST(Simulator, DISABLED_SingleWarpIssueRateIsOnePerCycle) {
  const KernelTrace k = synthetic::alu_independent(1, 100);
  EXPECT_GE(Simulator(SimConfig{}).run(k).cycles, 100u);
}

// RAW chain: each op must wait for the previous result, so N dependent ops take at
// least N * int_pipe.latency cycles and are strictly slower than the independent case.
TEST(Simulator, DISABLED_DependentChainIsSlowerThanIndependent) {
  SimConfig cfg;
  const auto dep = Simulator(cfg).run(synthetic::alu_dependent_chain(1, 100));
  const auto ind = Simulator(cfg).run(synthetic::alu_independent(1, 100));
  EXPECT_GE(dep.cycles, 100u * static_cast<Cycle>(cfg.int_pipe.latency));
  EXPECT_GT(dep.cycles, ind.cycles);
}

// Latency hiding: with enough warps, the dependent-chain kernel's IPC approaches
// the independent kernel's (the whole point of a GPU).
TEST(Simulator, DISABLED_ManyWarpsHideDependencyLatency) {
  SimConfig cfg;
  const auto one = Simulator(cfg).run(synthetic::alu_dependent_chain(1, 200));
  const auto many = Simulator(cfg).run(synthetic::alu_dependent_chain(16, 200));
  EXPECT_GT(many.ipc(), 4.0 * one.ipc());
}

// Strided loads touch 32 lines per warp access vs 1 for coalesced -> more cycles.
TEST(Simulator, DISABLED_UncoalescedLoadsAreSlower) {
  SimConfig cfg;
  const auto fast = Simulator(cfg).run(synthetic::global_loads(8, 50, 4));
  const auto slow = Simulator(cfg).run(synthetic::global_loads(8, 50, 128));
  EXPECT_GT(slow.cycles, fast.cycles);
}
