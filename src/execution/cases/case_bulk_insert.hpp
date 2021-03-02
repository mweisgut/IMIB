#pragma once

#include <stdexcept>

#include "abstract_benchmark_case.hpp"
#include "competitors/simple_vector.hpp"
#include "evaluation/memory_tracker.hpp"
#include "evaluation/timer.hpp"
#include "types.hpp"

namespace imib {

template <typename Index, typename Key, typename Value>
class CaseBulkInsert final : public AbstractBenchmarkCase<Index, Key, Value> {
 public:
  explicit CaseBulkInsert(const Data<Key, Value>& data) : AbstractBenchmarkCase<Index, Key, Value>(data) {
    // do nothing;
  }

  const std::string name() const override { return "BulkInsert"; }

  const ExecutionStatistics execute() const override {
    // benchmark scenario
    const auto memory_before = MemoryTracker::allocated_bytes();
    auto index = std::make_shared<Index>();

    Timer timer{};
    index->bulk_insert(this->_data);
    const auto duration = timer.lap();
    const auto memory_after = MemoryTracker::allocated_bytes();
    return {{{duration, memory_after - memory_before}}};
  }
};

}  // namespace imib
