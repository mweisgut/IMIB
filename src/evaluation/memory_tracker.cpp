#include "memory_tracker.hpp"

#include <cassert>
#include <stdexcept>

#ifdef IMIB_WITH_JEMALLOC
#include <jemalloc/jemalloc.h>
#endif

namespace imib {

size_t MemoryTracker::allocated_bytes() {
#ifdef IMIB_WITH_JEMALLOC
  // from http://jemalloc.net/jemalloc.3.html:
  //
  // epoch (uint64_t) rw
  // If a value is passed in, refresh the data from which the mallctl*() functions report values, and increment the
  // epoch. Return the current epoch. This is useful for detecting whether another thread caused a refresh.
  //
  // stats.allocated (size_t) r- [--enable-stats]
  // Total number of bytes allocated by the application.
  //
  // Before retrieving the statistics, we need to update jemalloc's epoch to get current values. See the mallctl
  // documentation for details.

  {
    [[maybe_unused]] auto error_code = mallctl("thread.tcache.flush", nullptr, nullptr, nullptr, 0);
    assert(!error_code);
  }

  {
    uint64_t epoch = 1;
    auto epoch_size = sizeof(epoch);
    [[maybe_unused]] auto error_code = mallctl("epoch", &epoch, &epoch_size, &epoch, epoch_size);
    assert(!error_code);
  }

  size_t allocated_bytes;
  auto allocated_size = sizeof(allocated_bytes);

  // malloc_stats_print(nullptr, nullptr, nullptr);
  [[maybe_unused]] auto error_code = mallctl("stats.allocated", &allocated_bytes, &allocated_size, nullptr, 0);
  assert(!error_code);

  return allocated_bytes;
#else
  throw std::logic_error("jemalloc is required for memory tracking.");
#endif
}

};  // namespace imib
