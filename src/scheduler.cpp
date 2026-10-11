#include "moonlight/scheduler.h"

namespace moonlight {
namespace {

// PLACEHOLDERS: both policies currently pick the lowest ready slot so the loop runs
// end-to-end. Replace with the real policies (week 1 RR, week 2 GTO).

class RoundRobinScheduler final : public WarpScheduler {
 public:
  int select(const std::vector<Warp>& warps, const std::vector<std::uint8_t>& ready,
             Cycle /*now*/) override {
    // TODO(week 1): start the search after last_issued_ and wrap around.
    for (std::size_t i = 0; i < warps.size(); ++i)
      if (ready[i]) return static_cast<int>(i);
    return -1;
  }
  void on_issue(int slot, Cycle /*now*/) override { last_issued_ = slot; }

 private:
  [[maybe_unused]] int last_issued_ = -1;
};

class GtoScheduler final : public WarpScheduler {
 public:
  int select(const std::vector<Warp>& warps, const std::vector<std::uint8_t>& ready,
             Cycle /*now*/) override {
    // TODO(week 2): keep issuing last_issued_ while it stays ready ("greedy");
    // otherwise pick the ready warp with the smallest Warp::age ("oldest").
    for (std::size_t i = 0; i < warps.size(); ++i)
      if (ready[i]) return static_cast<int>(i);
    return -1;
  }
  void on_issue(int slot, Cycle /*now*/) override { last_issued_ = slot; }

 private:
  [[maybe_unused]] int last_issued_ = -1;
};

}  // namespace

std::unique_ptr<WarpScheduler> make_scheduler(SchedulerPolicy policy) {
  switch (policy) {
    case SchedulerPolicy::RoundRobin: return std::make_unique<RoundRobinScheduler>();
    case SchedulerPolicy::GreedyThenOldest: return std::make_unique<GtoScheduler>();
  }
  return std::make_unique<GtoScheduler>();
}

}  // namespace moonlight
