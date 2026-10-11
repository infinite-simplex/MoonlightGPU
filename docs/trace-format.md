# Trace format

MoonlightGPU consumes **Accel-Sim style** traces, produced by Accel-Sim's NVBit tracer
(`accel-sim-framework/util/tracer_nvbit`) running on a real NVIDIA GPU.

> **Verify first.** This page was written from memory of the Accel-Sim format and has
> **not** been checked against a real trace file. On day 1, open a real `kernel-N.traceg`
> and fix any difference here and in `src/trace_reader.cpp`.

## Files

| File | Contents |
|---|---|
| `kernelslist.g` | One entry per line: a `kernel-N.traceg` path, or a `MemcpyHtoD,<addr>,<bytes>` command (ignore the memcpy lines for a timing-only simulator). |
| `kernel-N.traceg` | Header with kernel metadata, then per-thread-block, per-warp instruction records. |

## Header (key = value lines starting with `-`) 

Expected keys: `-kernel name`, `-kernel id`, `-grid dim`, `-block dim`, `-shmem`,
`-nregs`, plus some tracer/GPU info lines you can ignore. Map them to `KernelTrace`.

## Body

A comment line describes the column order, followed by blocks like:

```
#BEGIN_TB
thread block = 0,0,0
warp = 0
insts = 12
<one instruction record per line>
...
#END_TB
```

Each instruction record is roughly:

```
PC  active_mask  num_dst [dst regs]  OPCODE  num_src [src regs]  mem_width [address info]
```

- `PC`, `active_mask` are hex.
- Registers are named `R0..R254`, `RZ` (hard-wired zero), predicates `P0..P6`/`PT`,
  uniform registers `UR..`/`URZ`.
- For memory instructions (`mem_width > 0`) the address info starts with a format code:
  `0` = list every active lane's address, `1` = `base stride` (active lane k gets
  `base + k*stride`), `2` = `base` followed by per-lane deltas.

Map each record to a `TraceInst`. `mem_addrs` holds **one address per active lane in
ascending lane order**.

## Register id mapping (decision already made, see `types.h`)

| Trace name | `RegId` |
|---|---|
| `R0`..`R254` | 0..254 |
| `P0`..`P6` | 256..262 |
| `UR0`..`UR62` | 264..326 |
| `RZ`, `PT`, `URZ` | `kNoReg` (never a dependency) |

## Capturing traces (on the Linux GPU laptop)

1. Install the CUDA toolkit; clone `accel-sim-framework`.
2. Build the tracer: `./util/tracer_nvbit/install_nvbit.sh && make -C util/tracer_nvbit`.
3. Run your program under it, e.g.
   `LD_PRELOAD=./util/tracer_nvbit/tracer_tool/tracer_tool.so ./my_cuda_app`.
4. Keep problem sizes small (e.g. 256x256 SGEMM) -- traces get big fast. Compress with
   `xz` and copy to this machine under `traces/` (git-ignored).

Kernels worth tracing first: vector add, SGEMM, a **pointer-chase** (latency
calibration), and a streaming **copy** (bandwidth calibration).
