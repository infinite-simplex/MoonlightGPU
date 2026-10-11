#pragma once

#include <array>
#include <deque>
#include <memory>
#include <unordered_map>
#include <vector>

#include "moonlight/cache.h"
#include "moonlight/config.h"
#include "moonlight/pipeline.h"
#include "moonlight/scheduler.h"
#include "moonlight/scoreboard.h"
#include "moonlight/stats.h"
#include "moonlight/warp.h"

namespace moonlight {

// One streaming multiprocessor. tick() advances it by exactly one clock cycle.
class Sm {
 public:
  Sm(const SimConfig& cfg, Stats& stats);

  // Queue the kernel's thread blocks; `kernel` must outlive the Sm.
  void enqueue_blocks(const KernelTrace& kernel);

  void tick(Cycle now);

  // True while any block is pending or any warp is still resident.
  bool active() const;

 private:
  struct ResidentBlock {
    std::vector<int> slots;
    int live_warps = 0;
  };

  void admit_blocks();
  void compute_ready(Cycle now, int partition);
  void issue(int slot, Cycle now);
  void retire_blocks();

  SimConfig cfg_;
  Stats& stats_;

  std::deque<const ThreadBlockTrace*> pending_blocks_;
  std::vector<Warp> slots_;  // warp slot table, size = cfg.max_warps_per_sm
  std::unordered_map<std::uint32_t, ResidentBlock> resident_;
  std::uint32_t next_block_uid_ = 0;
  std::uint64_t next_age_ = 0;

  std::vector<std::unique_ptr<WarpScheduler>> schedulers_;  // one per partition
  std::vector<std::uint8_t> ready_;

  Scoreboard scoreboard_;
  std::array<ExecPipe, kNumPipes> pipes_;
  [[maybe_unused]] SetAssocCache l1_;  // TODO(week 3): wired up with the LSU
};

}  // namespace moonlight
