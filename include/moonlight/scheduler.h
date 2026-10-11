#pragma once

#include <memory>
#include <vector>

#include "moonlight/config.h"
#include "moonlight/warp.h"

namespace moonlight {

class WarpScheduler {
 public:
  virtual ~WarpScheduler() = default;

  // Pick the warp slot to issue this cycle, or -1 if none. `ready[i]` is nonzero if
  // warps[i] can issue now; the SM already zeroes entries outside this scheduler's
  // partition, so implementations just consider every ready slot.
  virtual int select(const std::vector<Warp>& warps, const std::vector<std::uint8_t>& ready,
                     Cycle now) = 0;

  // Called after the SM issued `slot` (lets policies update their state).
  virtual void on_issue(int slot, Cycle now) = 0;
};

std::unique_ptr<WarpScheduler> make_scheduler(SchedulerPolicy policy);

}  // namespace moonlight
