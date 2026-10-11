#pragma once

#include <cstdint>

#include "moonlight/types.h"

namespace moonlight {

enum class SchedulerPolicy { RoundRobin, GreedyThenOldest };

struct PipeConfig {
  int latency = 4;              // cycles from issue to result
  int initiation_interval = 2;  // min cycles between issues into this pipe
  int queue_depth = 8;          // max in-flight ops; full => structural hazard
};

struct CacheConfig {
  std::uint32_t size_bytes = 128 * 1024;
  std::uint32_t line_bytes = 128;
  std::uint32_t sector_bytes = 32;
  std::uint32_t assoc = 4;
  std::uint32_t hit_latency = 28;
  std::uint32_t mshr_entries = 64;
};

// Defaults are Volta-flavoured guesses (cf. Jia et al., "Dissecting the NVIDIA
// Volta GPU Architecture via Microbenchmarking"). They are NOT calibrated;
// calibrating them against your own microbenchmarks is a week-4/5 task.
struct SimConfig {
  int num_schedulers = 4;  // sub-cores per SM; warp slot s belongs to s % num_schedulers
  int max_warps_per_sm = 64;
  SchedulerPolicy policy = SchedulerPolicy::GreedyThenOldest;

  PipeConfig int_pipe{4, 2, 8};
  PipeConfig fp32_pipe{4, 2, 8};
  PipeConfig fp64_pipe{8, 8, 8};
  PipeConfig sfu_pipe{16, 8, 8};
  PipeConfig tensor_pipe{16, 8, 8};
  PipeConfig lsu_pipe{4, 1, 16};

  CacheConfig l1{};
  CacheConfig l2{4 * 1024 * 1024, 128, 32, 16, 193, 256};
  std::uint32_t dram_latency = 300;

  Cycle max_cycles = 200'000'000;  // deadlock watchdog
};

}  // namespace moonlight
