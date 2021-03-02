#pragma once
#include "abstract_benchmark_case.hpp"
#include "competitors/simple_vector.hpp"
#include "evaluation/memory_tracker.hpp"
#include "evaluation/timer.hpp"
#include "types.hpp"

namespace imib {

template <typename Index, typename Key, typename Value>
class CaseInsert final : public AbstractBenchmarkCase<Index, Key, Value> {
 public:
  explicit CaseInsert(const Data<Key, Value>& data) : AbstractBenchmarkCase<Index, Key, Value>(data) {
    // do nothing;
  }

  const std::string name() const override { return "Insert"; }

  const ExecutionStatistics execute() const override {
    // benchmark scenario
    const auto memory_before = MemoryTracker::allocated_bytes();
    auto index = std::make_shared<Index>();

    Timer timer{};
    for (auto& entry : this->_data.entries) {
      index->insert(entry.first, entry.second);
    }
    const auto duration = timer.lap();
    const auto memory_after = MemoryTracker::allocated_bytes();
    return {{{duration, memory_after - memory_before}}};
  }
};

}  // namespace imib
