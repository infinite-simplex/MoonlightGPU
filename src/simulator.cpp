#include "moonlight/simulator.h"

#include <stdexcept>

#include "moonlight/sm.h"

namespace moonlight {

Stats Simulator::run(const KernelTrace& kernel) {
  Stats stats;
  Sm sm(cfg_, stats);
  sm.enqueue_blocks(kernel);

  // The heart of the simulator: one iteration == one clock cycle.
  Cycle cycle = 0;
  while (sm.active()) {
    sm.tick(cycle);
    ++cycle;
    if (cycle >= cfg_.max_cycles)
      throw std::runtime_error("max_cycles exceeded: probable deadlock");
  }

  stats.cycles = cycle;
  return stats;
}

}  // namespace moonlight
