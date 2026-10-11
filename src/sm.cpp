#include "moonlight/sm.h"

#include <bit>
#include <stdexcept>

namespace moonlight {

namespace {

std::array<ExecPipe, kNumPipes> make_pipes(const SimConfig& c) {
  return {ExecPipe(c.int_pipe),  ExecPipe(c.fp32_pipe),   ExecPipe(c.fp64_pipe),
          ExecPipe(c.sfu_pipe),  ExecPipe(c.tensor_pipe), ExecPipe(c.lsu_pipe)};
}

}  // namespace

Sm::Sm(const SimConfig& cfg, Stats& stats)
    : cfg_(cfg),
      stats_(stats),
      slots_(static_cast<std::size_t>(cfg.max_warps_per_sm)),
      ready_(static_cast<std::size_t>(cfg.max_warps_per_sm), 0),
      scoreboard_(cfg.max_warps_per_sm),
      pipes_(make_pipes(cfg)),
      l1_(cfg.l1) {
  for (int i = 0; i < cfg_.max_warps_per_sm; ++i) slots_[static_cast<std::size_t>(i)].slot = i;
  for (int p = 0; p < cfg_.num_schedulers; ++p) schedulers_.push_back(make_scheduler(cfg_.policy));
}

void Sm::enqueue_blocks(const KernelTrace& kernel) {
  for (const auto& tb : kernel.blocks) {
    if (static_cast<int>(tb.warps.size()) > cfg_.max_warps_per_sm)
      throw std::runtime_error("thread block has more warps than the SM has warp slots");
    pending_blocks_.push_back(&tb);
  }
}

bool Sm::active() const { return !pending_blocks_.empty() || !resident_.empty(); }

void Sm::tick(Cycle now) {
  admit_blocks();

  // TODO(week 2): writeback first -- tick every pipe, release scoreboard entries for
  // completed ops -- so results are visible to issue in the same cycle they finish.

  for (int p = 0; p < cfg_.num_schedulers; ++p) {
    compute_ready(now, p);
    const int slot = schedulers_[static_cast<std::size_t>(p)]->select(slots_, ready_, now);
    if (slot >= 0) {
      issue(slot, now);
    } else {
      ++stats_.idle_issue_slots;
      // TODO(week 2): attribute the idle slot to the dominant Warp::stall in this partition.
      ++stats_.stall_slots[static_cast<std::size_t>(StallReason::NoWarps)];
    }
  }

  retire_blocks();
}

void Sm::admit_blocks() {
  while (!pending_blocks_.empty()) {
    const ThreadBlockTrace* tb = pending_blocks_.front();

    std::vector<int> free_slots;
    for (const Warp& w : slots_)
      if (!w.valid && free_slots.size() < tb->warps.size()) free_slots.push_back(w.slot);
    if (free_slots.size() < tb->warps.size()) return;  // wait for resources
    // TODO(later): also gate admission on shared memory / register file capacity.

    const std::uint32_t uid = next_block_uid_++;
    ResidentBlock& rb = resident_[uid];
    for (std::size_t i = 0; i < tb->warps.size(); ++i) {
      Warp& w = slots_[static_cast<std::size_t>(free_slots[i])];
      w = Warp{};
      w.valid = true;
      w.slot = free_slots[i];
      w.block_uid = uid;
      w.age = next_age_++;
      w.trace = &tb->warps[i];
      w.finished = w.trace->insts.empty();
      rb.slots.push_back(w.slot);
      if (!w.finished) ++rb.live_warps;
    }
    pending_blocks_.pop_front();
  }
}

void Sm::compute_ready(Cycle now, int partition) {
  for (Warp& w : slots_) {
    const auto i = static_cast<std::size_t>(w.slot);
    ready_[i] = 0;
    if (!w.valid || w.finished || w.slot % cfg_.num_schedulers != partition) continue;

    const TraceInst* inst = w.next_inst();
    w.stall = StallReason::None;
    // TODO(week 2): if (!scoreboard_.can_issue(w.slot, *inst)) { w.stall = Scoreboard; continue; }
    // TODO(week 2): if (!pipes_[pipe_for(inst->op_class)].can_accept(now)) { w.stall = PipeBusy; continue; }
    // TODO(week 2/3): barriers (Barrier) and LSU/MSHR back-pressure (MemWait).
    (void)inst;
    (void)now;
    ready_[i] = 1;
  }
}

void Sm::issue(int slot, Cycle now) {
  Warp& w = slots_[static_cast<std::size_t>(slot)];
  const TraceInst& inst = *w.next_inst();

  // PLACEHOLDER: every instruction completes the cycle it issues.
  // TODO(week 2): scoreboard_.reserve(slot, inst); push an InFlightOp into
  //   pipes_[pipe_for(inst.op_class)] with complete_at = now + latency.
  // TODO(week 3): for memory ops, coalesce() and send the transactions through l1_.
  ++stats_.warp_insts_issued;
  stats_.thread_insts += static_cast<std::uint64_t>(std::popcount(inst.active_mask));

  ++w.pc;
  if (w.pc >= w.trace->insts.size()) {
    w.finished = true;
    --resident_.at(w.block_uid).live_warps;
  }
  schedulers_[static_cast<std::size_t>(slot % cfg_.num_schedulers)]->on_issue(slot, now);
}

void Sm::retire_blocks() {
  for (auto it = resident_.begin(); it != resident_.end();) {
    if (it->second.live_warps == 0) {
      for (int s : it->second.slots) {
        scoreboard_.reset(s);
        slots_[static_cast<std::size_t>(s)] = Warp{};
        slots_[static_cast<std::size_t>(s)].slot = s;
      }
      it = resident_.erase(it);
    } else {
      ++it;
    }
  }
}

}  // namespace moonlight
