#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "moonlight/types.h"

namespace moonlight {

enum class OpClass : std::uint8_t {
  IntAlu,
  Fp32,
  Fp64,
  Sfu,
  Tensor,
  GlobalLoad,
  GlobalStore,
  GlobalAtomic,
  SharedLoad,
  SharedStore,
  ConstLoad,
  Branch,
  Barrier,
  Exit,
  Count
};

const char* to_string(OpClass c);
bool is_memory(OpClass c);

// Maps a SASS opcode string ("FFMA", "LDG.E.SYS", "HMMA.1688.F32") to a class.
OpClass classify_opcode(std::string_view sass_opcode);

constexpr int kMaxDst = 2;
constexpr int kMaxSrc = 5;

// One warp-level dynamic instruction, as emitted by the tracer.
struct TraceInst {
  std::uint64_t pc = 0;
  LaneMask active_mask = 0;
  OpClass op_class = OpClass::IntAlu;
  std::uint16_t opcode_id = 0;  // index into KernelTrace::opcode_names
  std::uint8_t num_dst = 0;
  std::uint8_t num_src = 0;
  std::array<RegId, kMaxDst> dst{kNoReg, kNoReg};
  std::array<RegId, kMaxSrc> src{kNoReg, kNoReg, kNoReg, kNoReg, kNoReg};
  std::uint8_t mem_width = 0;  // bytes per thread; 0 for non-memory
  // One address per ACTIVE lane, in ascending lane order. Empty if non-memory.
  std::vector<Addr> mem_addrs;
};

struct WarpTrace {
  std::vector<TraceInst> insts;
};

struct Dim3 {
  std::uint32_t x = 1, y = 1, z = 1;
};

struct ThreadBlockTrace {
  Dim3 id;
  std::vector<WarpTrace> warps;
};

struct KernelTrace {
  std::string name;
  std::uint32_t kernel_id = 0;
  Dim3 grid;
  Dim3 block;
  std::uint32_t shmem_bytes = 0;
  std::uint32_t num_regs = 0;
  std::vector<std::string> opcode_names;  // interned opcode strings
  std::vector<ThreadBlockTrace> blocks;

  std::uint16_t intern_opcode(std::string_view opcode);
  std::uint64_t total_warp_insts() const;
};

}  // namespace moonlight
