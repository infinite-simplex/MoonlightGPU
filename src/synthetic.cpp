#include "moonlight/synthetic.h"

namespace moonlight::synthetic {

TraceInst make_inst(KernelTrace& k, std::string_view opcode, std::uint64_t pc,
                    std::initializer_list<RegId> dst, std::initializer_list<RegId> src,
                    LaneMask mask) {
  TraceInst i;
  i.pc = pc;
  i.active_mask = mask;
  i.op_class = classify_opcode(opcode);
  i.opcode_id = k.intern_opcode(opcode);
  for (RegId r : dst) i.dst[i.num_dst++] = r;
  for (RegId r : src) i.src[i.num_src++] = r;
  return i;
}

namespace {

KernelTrace skeleton(int num_warps, const char* name) {
  KernelTrace k;
  k.name = name;
  k.block = {static_cast<std::uint32_t>(num_warps * kWarpSize), 1, 1};
  k.blocks.resize(1);
  k.blocks[0].warps.resize(static_cast<std::size_t>(num_warps));
  return k;
}

}  // namespace

KernelTrace alu_independent(int num_warps, int n) {
  KernelTrace k = skeleton(num_warps, "alu_independent");
  for (auto& w : k.blocks[0].warps)
    for (int i = 0; i < n; ++i)
      w.insts.push_back(
          make_inst(k, "IADD3", 16u * static_cast<unsigned>(i), {static_cast<RegId>(1 + i % 200)}, {}));
  return k;
}

KernelTrace alu_dependent_chain(int num_warps, int n) {
  KernelTrace k = skeleton(num_warps, "alu_dependent_chain");
  for (auto& w : k.blocks[0].warps)
    for (int i = 0; i < n; ++i)
      w.insts.push_back(make_inst(k, "IADD3", 16u * static_cast<unsigned>(i), {1}, {1}));
  return k;
}

KernelTrace global_loads(int num_warps, int n, std::uint32_t stride_bytes) {
  KernelTrace k = skeleton(num_warps, "global_loads");
  for (int wi = 0; wi < num_warps; ++wi) {
    auto& w = k.blocks[0].warps[static_cast<std::size_t>(wi)];
    for (int i = 0; i < n; ++i) {
      TraceInst inst = make_inst(k, "LDG.E", 16u * static_cast<unsigned>(i),
                                 {static_cast<RegId>(2 + i % 100)}, {1});
      inst.mem_width = 4;
      const Addr base = 0x7f0000000000ull + (static_cast<Addr>(wi) * static_cast<Addr>(n) +
                                             static_cast<Addr>(i)) * 0x10000ull;
      for (int lane = 0; lane < kWarpSize; ++lane)
        inst.mem_addrs.push_back(base + static_cast<Addr>(lane) * stride_bytes);
      w.insts.push_back(std::move(inst));
    }
  }
  return k;
}

}  // namespace moonlight::synthetic
