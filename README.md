# MoonlightGPU

A trace-driven, cycle-accurate GPU microarchitecture timing simulator in C++20.

It does **not** emulate SASS or compute results. It replays instruction traces captured
from a real NVIDIA GPU (via NVBit / the Accel-Sim tracer) and models the hardware
constraints that determine performance: warp scheduling, register scoreboarding,
execution-pipe contention, memory coalescing and the cache hierarchy.

> Status: skeleton. See [docs/ROADMAP.md](docs/ROADMAP.md).

## Architecture

```
trace file --> TraceReader --> KernelTrace (blocks -> warps -> TraceInst)
                                   |
Simulator::run:  while (sm.active()) { sm.tick(cycle); ++cycle; }
                                   |
                  Sm::tick: admit blocks -> [writeback] -> per scheduler:
                            compute ready warps -> WarpScheduler::select -> issue
                                   |
              Scoreboard   ExecPipe x6 (int/fp32/fp64/sfu/tensor/lsu)   coalescer -> L1 -> L2 -> DRAM
```

| Module | Files |
|---|---|
| Trace model + reader | `trace.h`, `trace_reader.h` |
| Warp scheduler (RR / GTO) | `scheduler.h`, `warp.h` |
| Scoreboard | `scoreboard.h` |
| Execution pipes | `pipeline.h` |
| Coalescer / caches | `coalescer.h`, `cache.h` |
| SM and top-level loop | `sm.h`, `simulator.h` |
| Parameters | `config.h` |

## Build

Requires CMake >= 3.20 and a C++20 compiler. Tests fetch GoogleTest (needs network on first configure).

```bash
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
./build/Release/moonlight --synthetic alu     # build/moonlight on Linux/Ninja
```

Windows note: the standalone CMake 3.30 does not know the Visual Studio 2026 generator.
Use the CMake bundled with Visual Studio
(`...\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe`) or any newer
CMake whose `cmake --help` lists "Visual Studio 18 2026".

## Traces

See [docs/trace-format.md](docs/trace-format.md). Traces live in `traces/` (git-ignored).
`moonlight --synthetic alu|chain|coalesced|strided` runs built-in synthetic kernels with no trace file.
