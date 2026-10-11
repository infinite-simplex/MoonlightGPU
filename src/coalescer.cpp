#include "moonlight/coalescer.h"

namespace moonlight {

std::vector<MemTransaction> coalesce(const TraceInst& inst, std::uint32_t /*line_bytes*/,
                                     std::uint32_t /*sector_bytes*/) {
  if (!is_memory(inst.op_class)) return {};
  // TODO(week 3): group inst.mem_addrs (+ mem_width) by line, OR the touched sectors
  // into each line's sector_mask, and handle accesses that straddle a line boundary.
  return {};
}

}  // namespace moonlight
