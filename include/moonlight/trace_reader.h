#pragma once

#include <filesystem>
#include <vector>

#include "moonlight/trace.h"

namespace moonlight {

// Parses one Accel-Sim style kernel trace (kernel-N.traceg). Format notes live in
// docs/trace-format.md. Throws std::runtime_error on malformed input.
KernelTrace read_kernel_trace(const std::filesystem::path& file);

// Parses a kernelslist.g and returns the kernel trace paths it references.
std::vector<std::filesystem::path> read_kernel_list(const std::filesystem::path& file);

}  // namespace moonlight
