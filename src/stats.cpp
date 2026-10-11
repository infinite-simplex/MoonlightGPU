#include "moonlight/stats.h"

#include <iomanip>
#include <ostream>

namespace moonlight {

const char* to_string(StallReason r) {
  switch (r) {
    case StallReason::None: return "none";
    case StallReason::Scoreboard: return "scoreboard";
    case StallReason::PipeBusy: return "pipe_busy";
    case StallReason::Barrier: return "barrier";
    case StallReason::MemWait: return "mem_wait";
    case StallReason::NoWarps: return "no_warps";
    case StallReason::Count: break;
  }
  return "?";
}

void Stats::print(std::ostream& os) const {
  os << "cycles:            " << cycles << "\n"
     << "warp insts issued: " << warp_insts_issued << "\n"
     << "thread insts:      " << thread_insts << "\n"
     << "IPC (warp):        " << std::fixed << std::setprecision(3) << ipc() << "\n"
     << "idle issue slots:  " << idle_issue_slots << "\n";
  for (std::size_t i = 1; i < stall_slots.size(); ++i)
    if (stall_slots[i])
      os << "  stall[" << to_string(static_cast<StallReason>(i)) << "]: " << stall_slots[i] << "\n";
}

}  // namespace moonlight
