#include "moonlight/trace.h"

#include <algorithm>

namespace moonlight {

const char* to_string(OpClass c) {
  switch (c) {
    case OpClass::IntAlu: return "IntAlu";
    case OpClass::Fp32: return "Fp32";
    case OpClass::Fp64: return "Fp64";
    case OpClass::Sfu: return "Sfu";
    case OpClass::Tensor: return "Tensor";
    case OpClass::GlobalLoad: return "GlobalLoad";
    case OpClass::GlobalStore: return "GlobalStore";
    case OpClass::GlobalAtomic: return "GlobalAtomic";
    case OpClass::SharedLoad: return "SharedLoad";
    case OpClass::SharedStore: return "SharedStore";
    case OpClass::ConstLoad: return "ConstLoad";
    case OpClass::Branch: return "Branch";
    case OpClass::Barrier: return "Barrier";
    case OpClass::Exit: return "Exit";
    case OpClass::Count: break;
  }
  return "?";
}

bool is_memory(OpClass c) {
  switch (c) {
    case OpClass::GlobalLoad:
    case OpClass::GlobalStore:
    case OpClass::GlobalAtomic:
    case OpClass::SharedLoad:
    case OpClass::SharedStore:
    case OpClass::ConstLoad:
      return true;
    default:
      return false;
  }
}

OpClass classify_opcode(std::string_view sass) {
  const std::string_view base = sass.substr(0, sass.find('.'));

  auto is = [&](std::initializer_list<std::string_view> names) {
    return std::find(names.begin(), names.end(), base) != names.end();
  };

  // Local-memory ops (LDL/STL) are treated as global for now.
  if (is({"LDG", "LD", "LDL"})) return OpClass::GlobalLoad;
  if (is({"STG", "ST", "STL"})) return OpClass::GlobalStore;
  if (is({"ATOMG", "ATOM", "RED"})) return OpClass::GlobalAtomic;
  if (is({"LDS", "LDSM"})) return OpClass::SharedLoad;
  if (is({"STS"})) return OpClass::SharedStore;
  if (is({"LDC"})) return OpClass::ConstLoad;
  if (is({"HMMA", "IMMA", "BMMA", "DMMA"})) return OpClass::Tensor;
  if (is({"MUFU"})) return OpClass::Sfu;
  if (is({"DFMA", "DADD", "DMUL", "DSETP", "DMNMX"})) return OpClass::Fp64;
  if (is({"FFMA", "FADD", "FMUL", "FSETP", "FMNMX", "FSEL", "FCHK", "HFMA2", "HADD2", "HMUL2"}))
    return OpClass::Fp32;
  if (is({"BAR"})) return OpClass::Barrier;
  if (is({"EXIT"})) return OpClass::Exit;
  if (is({"BRA", "BRX", "JMP", "JMX", "CALL", "RET", "BSSY", "BSYNC", "WARPSYNC", "YIELD"}))
    return OpClass::Branch;
  return OpClass::IntAlu;  // IMAD, IADD3, LOP3, ISETP, MOV, SHF, S2R, LEA, SEL, ...
}

std::uint16_t KernelTrace::intern_opcode(std::string_view opcode) {
  for (std::size_t i = 0; i < opcode_names.size(); ++i)
    if (opcode_names[i] == opcode) return static_cast<std::uint16_t>(i);
  opcode_names.emplace_back(opcode);
  return static_cast<std::uint16_t>(opcode_names.size() - 1);
}

std::uint64_t KernelTrace::total_warp_insts() const {
  std::uint64_t n = 0;
  for (const auto& b : blocks)
    for (const auto& w : b.warps) n += w.insts.size();
  return n;
}

}  // namespace moonlight
