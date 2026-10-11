#include <cstdlib>
#include <iostream>
#include <string>

#include "moonlight/simulator.h"
#include "moonlight/synthetic.h"
#include "moonlight/trace_reader.h"

using namespace moonlight;

namespace {

void usage() {
  std::cerr << "usage: moonlight [--sched rr|gto] [--max-cycles N]\n"
               "                 (<kernel.traceg> | --synthetic alu|chain|coalesced|strided)\n";
}

}  // namespace

int main(int argc, char** argv) {
  SimConfig cfg;
  std::string trace_path;
  std::string synthetic_name;

  for (int i = 1; i < argc; ++i) {
    const std::string a = argv[i];
    if (a == "--sched" && i + 1 < argc) {
      const std::string v = argv[++i];
      cfg.policy = (v == "rr") ? SchedulerPolicy::RoundRobin : SchedulerPolicy::GreedyThenOldest;
    } else if (a == "--max-cycles" && i + 1 < argc) {
      cfg.max_cycles = std::strtoull(argv[++i], nullptr, 10);
    } else if (a == "--synthetic" && i + 1 < argc) {
      synthetic_name = argv[++i];
    } else if (!a.empty() && a[0] != '-') {
      trace_path = a;
    } else {
      usage();
      return 2;
    }
  }

  try {
    KernelTrace kernel;
    if (!synthetic_name.empty()) {
      if (synthetic_name == "alu") kernel = synthetic::alu_independent(8, 1000);
      else if (synthetic_name == "chain") kernel = synthetic::alu_dependent_chain(8, 1000);
      else if (synthetic_name == "coalesced") kernel = synthetic::global_loads(8, 100, 4);
      else if (synthetic_name == "strided") kernel = synthetic::global_loads(8, 100, 128);
      else { usage(); return 2; }
    } else if (!trace_path.empty()) {
      kernel = read_kernel_trace(trace_path);
    } else {
      usage();
      return 2;
    }

    const Stats stats = Simulator(cfg).run(kernel);
    std::cout << "kernel: " << kernel.name << "\n";
    stats.print(std::cout);
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
