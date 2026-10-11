#pragma once

#include <array>
#include <deque>
#include <vector>

#include "moonlight/config.h"
#include "moonlight/trace.h"

namespace moonlight {

enum class PipeId : std::uint8_t { Int, Fp32, Fp64, Sfu, Tensor, Lsu, Count };
inline constexpr std::size_t kNumPipes = static_cast<std::size_t>(PipeId::Count);

// Which execution unit services an instruction class.
PipeId pipe_for(OpClass c);

struct InFlightOp {
  int warp_slot = -1;
  const TraceInst* inst = nullptr;
  Cycle complete_at = 0;
};

// A multi-cycle execution unit modelled as a bounded queue.
class ExecPipe {
 public:
  explicit ExecPipe(const PipeConfig& cfg);

  // False => structural hazard (queue full or initiation interval not elapsed).
  bool can_accept(Cycle now) const;

  void issue(const InFlightOp& op, Cycle now);

  // Appends ops that finish at or before `now` to `completed`.
  void tick(Cycle now, std::vector<InFlightOp>& completed);

  bool empty() const { return in_flight_.empty(); }

 private:
  PipeConfig cfg_;
  std::deque<InFlightOp> in_flight_;
  // TODO(week 2): track last issue cycle for the initiation interval.
};

}  // namespace moonlight
