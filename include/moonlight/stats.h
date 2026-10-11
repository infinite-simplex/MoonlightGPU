#pragma once

#include <array>
#include <cstdint>
#include <iosfwd>

#include "moonlight/types.h"

namespace moonlight {

enum class StallReason : std::uint8_t {
  None,
  Scoreboard,  // RAW/WAW dependency outstanding
  PipeBusy,    // structural hazard: target pipe full / initiation interval
  Barrier,     // waiting at a block barrier
  MemWait,     // waiting on memory (MSHRs full, LSU queue full, ...)
  NoWarps,     // scheduler has no resident warps
  Count
};

const char* to_string(StallReason r);

struct Stats {
  Cycle cycles = 0;
  std::uint64_t warp_insts_issued = 0;
  std::uint64_t thread_insts = 0;       // sum of popcount(active_mask)
  std::uint64_t idle_issue_slots = 0;   // scheduler-cycles with nothing issued
  std::array<std::uint64_t, static_cast<std::size_t>(StallReason::Count)> stall_slots{};

  double ipc() const { return cycles ? double(warp_insts_issued) / double(cycles) : 0.0; }
  void print(std::ostream& os) const;
};

}  // namespace moonlight
