#include "moonlight/trace_reader.h"

#include <stdexcept>

namespace moonlight {

KernelTrace read_kernel_trace(const std::filesystem::path& file) {
  // TODO(week 1): parse the header ("-kernel name = ...", "-grid dim = ...", ...) and
  // the per-warp instruction lines described in docs/trace-format.md.
  throw std::runtime_error("read_kernel_trace not implemented yet: " + file.string());
}

std::vector<std::filesystem::path> read_kernel_list(const std::filesystem::path& file) {
  // TODO(week 1): each non-comment line of kernelslist.g is either a
  // "kernel-N.traceg" path or a "MemcpyHtoD,..." command (ignore the latter).
  throw std::runtime_error("read_kernel_list not implemented yet: " + file.string());
}

}  // namespace moonlight
