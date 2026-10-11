# Roadmap (target: ~6 weeks, 20-30 h/week)

Each item lists the stubs (`TODO(week N)` in the code) and the `DISABLED_` tests to
enable. Rule of thumb: enable a test, watch it fail, implement, watch it pass.

## Days 1-2: setup
- [ ] Linux laptop: build the Accel-Sim tracer, capture vector-add, small SGEMM, pointer-chase, copy (see `docs/trace-format.md`)
- [ ] Also download Accel-Sim's published traces as a fallback
- [ ] Open a real `.traceg`; correct `docs/trace-format.md`

## Week 1: reader, loop, round-robin
- [ ] `read_kernel_trace` / `read_kernel_list` (`src/trace_reader.cpp`) + a tiny test file in `tests/data/`
- [ ] Real `RoundRobinScheduler` (`src/scheduler.cpp`)
- [ ] Enable: `Scheduler.RoundRobin*`, `Simulator.SingleWarpIssueRateIsOnePerCycle`

## Week 2: GTO, scoreboard, pipelines
- [ ] `GtoScheduler`
- [ ] `Scoreboard` (per-slot pending bitset over `kNumRegIds`)
- [ ] `ExecPipe`: latency, initiation interval, queue depth; wire into `Sm::issue` / writeback / `compute_ready`
- [ ] Stall attribution in `Sm::tick` (`StallReason`)
- [ ] Enable: `Scheduler.Gto*`, `Scoreboard.*`, `Pipeline.*`, `Simulator.Dependent*`, `Simulator.ManyWarps*`

## Week 3: coalescing + L1  -> resume-ready MVP
- [ ] `coalesce()` (`src/coalescer.cpp`)
- [ ] `SetAssocCache`: sectored, LRU
- [ ] LSU path: coalesce -> L1 -> fixed-latency L2/DRAM
- [ ] Enable: `Coalescer.*`, `Cache.*`, `Simulator.UncoalescedLoadsAreSlower`
- [ ] Push a README with first results; start applying

## Week 4: L2 + MSHRs, calibration
- [ ] L2, MSHRs, miss queues, bandwidth limits
- [ ] Calibrate latencies against the pointer-chase microbenchmark

## Week 5: experiments + hardware validation
- [ ] Sweeps: GTO vs RR, L1 size, coalesced vs strided -> CSV + plots
- [ ] Simulated vs measured cycles on the real GPU (error plot)

## Week 6: polish
- [ ] README with architecture diagram, results, limitations
- [ ] CI (GitHub Actions: build + ctest on Linux)
- [ ] Simulator speed (simulated cycles/sec), profiling pass

## Deliberately out of scope (unless time remains)
Multi-SM, detailed tensor-core internals, multiple cache replacement policies, branch divergence reconstruction.
