#pragma once

#include "moonlight/trace.h"

namespace moonlight::synthetic {

// Single thread block, `num_warps` warps, `n` independent integer ops per warp
// (no register dependencies).
KernelTrace alu_independent(int num_warps, int n);

// Same, but every op reads the previous op's result (RAW chain on one register).
KernelTrace alu_dependent_chain(int num_warps, int n);

// Each warp issues `n` global loads; lane i reads base + i * stride_bytes.
// stride 4 => fully coalesced, stride 128 => one line per lane.
KernelTrace global_loads(int num_warps, int n, std::uint32_t stride_bytes);

// Builder helpers for hand-written test traces.
TraceInst make_inst(KernelTrace& k, std::string_view opcode, std::uint64_t pc,
                    std::initializer_list<RegId> dst, std::initializer_list<RegId> src,
                    LaneMask mask = 0xffffffffu);

}  // namespace moonlight::synthetic
