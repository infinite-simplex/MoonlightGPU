#pragma once

#include <vector>

#include "moonlight/trace.h"

namespace moonlight {

// Per-warp register scoreboard. Indexed by warp SLOT (stable for a warp's lifetime).
class Scoreboard {
 public:
  explicit Scoreboard(int num_warp_slots);

  // True if `inst` has no outstanding RAW (src) or WAW (dst) hazard for this warp.
  bool can_issue(int slot, const TraceInst& inst) const;

  // Mark inst's destination registers as pending (call at issue).
  void reserve(int slot, const TraceInst& inst);

  // Clear one pending register (call at writeback).
  void release(int slot, RegId reg);

  // Clear everything for a slot (warp/block retired).
  void reset(int slot);

 private:
  // TODO(week 2): per-slot pending bitset over kNumRegIds.
  int num_slots_;
};

}  // namespace moonlight
