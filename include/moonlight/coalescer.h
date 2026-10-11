#pragma once

#include <cstdint>
#include <vector>

#include "moonlight/trace.h"

namespace moonlight {

// One request to the cache hierarchy produced from a warp memory instruction.
struct MemTransaction {
  Addr line_addr = 0;            // aligned to line_bytes
  std::uint32_t sector_mask = 0; // bit i => sector i of the line is touched
  bool is_write = false;
};

// Merge the active lanes' addresses of `inst` into the minimal set of line/sector
// transactions. Returns an empty vector for non-memory instructions.
std::vector<MemTransaction> coalesce(const TraceInst& inst, std::uint32_t line_bytes,
                                     std::uint32_t sector_bytes);

}  // namespace moonlight
