#include "moonlight/scoreboard.h"

namespace moonlight {

Scoreboard::Scoreboard(int num_warp_slots) : num_slots_(num_warp_slots) {}

bool Scoreboard::can_issue(int /*slot*/, const TraceInst& /*inst*/) const {
  // TODO(week 2): false if any src or dst reg of `inst` is pending for this slot.
  return true;
}

void Scoreboard::reserve(int /*slot*/, const TraceInst& /*inst*/) {
  // TODO(week 2)
}

void Scoreboard::release(int /*slot*/, RegId /*reg*/) {
  // TODO(week 2)
}

void Scoreboard::reset(int /*slot*/) {
  // TODO(week 2)
}

}  // namespace moonlight
