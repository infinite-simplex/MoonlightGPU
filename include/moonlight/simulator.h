#pragma once

#include "moonlight/config.h"
#include "moonlight/stats.h"
#include "moonlight/trace.h"

namespace moonlight {

class Simulator {
 public:
  explicit Simulator(SimConfig cfg) : cfg_(cfg) {}

  // Runs one kernel to completion: one loop iteration == one clock cycle.
  // Throws std::runtime_error if cfg.max_cycles is exceeded (deadlock watchdog).
  Stats run(const KernelTrace& kernel);

 private:
  SimConfig cfg_;
};

}  // namespace moonlight
