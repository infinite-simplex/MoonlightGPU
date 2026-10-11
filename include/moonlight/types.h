#pragma once

#include <cstdint>

namespace moonlight {

inline constexpr int kWarpSize = 32;

using Cycle = std::uint64_t;
using Addr = std::uint64_t;
using LaneMask = std::uint32_t;  // bit i set => lane i active

// Unified register id space used by the scoreboard (see docs/trace-format.md):
//   R0..R254   -> 0..254
//   P0..P6     -> 256..262
//   UR0..UR62  -> 264..326
//   RZ/PT/URZ  -> kNoReg (hard-wired, never a dependency)
using RegId = std::int16_t;
inline constexpr RegId kNoReg = -1;
inline constexpr int kNumRegIds = 384;

}  // namespace moonlight
