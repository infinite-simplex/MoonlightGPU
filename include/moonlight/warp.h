#pragma once

#include <cstddef>
#include <cstdint>

#include "moonlight/stats.h"
#include "moonlight/trace.h"

namespace moonlight {

// One entry in the SM's warp slot table.
struct Warp {
  bool valid = false;     // slot is occupied
  bool finished = false;  // consumed its whole trace
  int slot = -1;
  std::uint32_t block_uid = 0;
  std::uint64_t age = 0;  // admission order; smaller == older (for GTO)

  const WarpTrace* trace = nullptr;
  std::size_t pc = 0;  // index of the next instruction in trace->insts

  StallReason stall = StallReason::None;  // why it could not issue last cycle

  const TraceInst* next_inst() const {
    return (trace && pc < trace->insts.size()) ? &trace->insts[pc] : nullptr;
  }
};

}  // namespace moonlight
