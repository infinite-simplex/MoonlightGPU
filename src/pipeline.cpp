#include "moonlight/pipeline.h"

namespace moonlight {

PipeId pipe_for(OpClass c) {
  switch (c) {
    case OpClass::Fp32: return PipeId::Fp32;
    case OpClass::Fp64: return PipeId::Fp64;
    case OpClass::Sfu: return PipeId::Sfu;
    case OpClass::Tensor: return PipeId::Tensor;
    case OpClass::GlobalLoad:
    case OpClass::GlobalStore:
    case OpClass::GlobalAtomic:
    case OpClass::SharedLoad:
    case OpClass::SharedStore:
    case OpClass::ConstLoad:
      return PipeId::Lsu;
    default:
      return PipeId::Int;  // IntAlu, Branch, Barrier, Exit
  }
}

ExecPipe::ExecPipe(const PipeConfig& cfg) : cfg_(cfg) {}

bool ExecPipe::can_accept(Cycle /*now*/) const {
  // TODO(week 2): false if in_flight_ is at cfg_.queue_depth, or fewer than
  // cfg_.initiation_interval cycles have passed since the last issue.
  return true;
}

void ExecPipe::issue(const InFlightOp& op, Cycle /*now*/) {
  // TODO(week 2): stamp op.complete_at = now + cfg_.latency (caller or here).
  in_flight_.push_back(op);
}

void ExecPipe::tick(Cycle now, std::vector<InFlightOp>& completed) {
  while (!in_flight_.empty() && in_flight_.front().complete_at <= now) {
    completed.push_back(in_flight_.front());
    in_flight_.pop_front();
  }
}

}  // namespace moonlight
