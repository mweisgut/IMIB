#pragma once

#include <cstddef>

namespace imib {

class MemoryTracker final {
 public:
  MemoryTracker() = delete;
  // returns the current amount of allocated memory in bytes
  static size_t allocated_bytes();
};

}  // namespace imib
