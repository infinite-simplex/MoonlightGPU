#include <gtest/gtest.h>

#include "moonlight/synthetic.h"
#include "moonlight/trace.h"

using namespace moonlight;

TEST(Trace, ClassifyOpcodes) {
  EXPECT_EQ(classify_opcode("FFMA"), OpClass::Fp32);
  EXPECT_EQ(classify_opcode("IMAD.MOV.U32"), OpClass::IntAlu);
  EXPECT_EQ(classify_opcode("LDG.E.SYS"), OpClass::GlobalLoad);
  EXPECT_EQ(classify_opcode("STG.E"), OpClass::GlobalStore);
  EXPECT_EQ(classify_opcode("LDS.U.128"), OpClass::SharedLoad);
  EXPECT_EQ(classify_opcode("HMMA.1688.F32"), OpClass::Tensor);
  EXPECT_EQ(classify_opcode("MUFU.RCP"), OpClass::Sfu);
  EXPECT_EQ(classify_opcode("DFMA"), OpClass::Fp64);
  EXPECT_EQ(classify_opcode("BAR.SYNC"), OpClass::Barrier);
  EXPECT_EQ(classify_opcode("EXIT"), OpClass::Exit);
}

TEST(Trace, InternOpcodeIsStable) {
  KernelTrace k;
  const auto a = k.intern_opcode("FFMA");
  const auto b = k.intern_opcode("IADD3");
  EXPECT_NE(a, b);
  EXPECT_EQ(k.intern_opcode("FFMA"), a);
  EXPECT_EQ(k.opcode_names.size(), 2u);
}

TEST(Trace, SyntheticGlobalLoadsHaveOneAddressPerActiveLane) {
  const KernelTrace k = synthetic::global_loads(2, 3, 4);
  EXPECT_EQ(k.total_warp_insts(), 6u);
  for (const auto& w : k.blocks[0].warps)
    for (const auto& i : w.insts) {
      EXPECT_EQ(i.op_class, OpClass::GlobalLoad);
      EXPECT_EQ(i.mem_addrs.size(), 32u);
    }
}

// TODO(week 1): once read_kernel_trace exists, add a tiny hand-written .traceg under
// tests/data/ and assert on the parsed header, instruction count and addresses.
