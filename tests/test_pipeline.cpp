#include <gtest/gtest.h>

#include "moonlight/pipeline.h"

using namespace moonlight;

TEST(Pipeline, OpClassesMapToExpectedPipes) {
  EXPECT_EQ(pipe_for(OpClass::IntAlu), PipeId::Int);
  EXPECT_EQ(pipe_for(OpClass::Fp32), PipeId::Fp32);
  EXPECT_EQ(pipe_for(OpClass::Tensor), PipeId::Tensor);
  EXPECT_EQ(pipe_for(OpClass::GlobalLoad), PipeId::Lsu);
  EXPECT_EQ(pipe_for(OpClass::SharedStore), PipeId::Lsu);
}

TEST(Pipeline, DISABLED_OpCompletesAfterLatency) {
  PipeConfig cfg{4, 1, 8};
  ExecPipe p(cfg);
  p.issue(InFlightOp{0, nullptr, 0}, /*now=*/10);
  std::vector<InFlightOp> done;
  p.tick(13, done);
  EXPECT_TRUE(done.empty());
  p.tick(14, done);
  EXPECT_EQ(done.size(), 1u);
}

TEST(Pipeline, DISABLED_QueueDepthCausesStructuralHazard) {
  PipeConfig cfg{100, 1, 2};
  ExecPipe p(cfg);
  p.issue(InFlightOp{0, nullptr, 0}, 0);
  p.issue(InFlightOp{1, nullptr, 0}, 1);
  EXPECT_FALSE(p.can_accept(2));
}

TEST(Pipeline, DISABLED_InitiationIntervalLimitsIssueRate) {
  PipeConfig cfg{4, 2, 8};  // one issue every 2 cycles
  ExecPipe p(cfg);
  p.issue(InFlightOp{0, nullptr, 0}, 0);
  EXPECT_FALSE(p.can_accept(1));
  EXPECT_TRUE(p.can_accept(2));
}
