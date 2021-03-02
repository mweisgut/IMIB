#pragma once

#include <stdexcept>

#include "abstract_benchmark_case.hpp"
#include "competitors/simple_vector.hpp"
#include "evaluation/memory_tracker.hpp"
#include "evaluation/timer.hpp"
#include "types.hpp"

namespace imib {

template <typename Index, typename Key, typename Value>
class CaseErase final : public AbstractBenchmarkCase<Index, Key, Value> {
 public:
  explicit CaseErase(const Data<Key, Value>& data) : AbstractBenchmarkCase<Index, Key, Value>(data) {
    // do nothing;
  }

  const std::string name() const override { return "Erase"; }

  // only time measurement
  const ExecutionStatistics execute() const override {
    // preparation
    auto index = std::make_shared<Index>();

    for (auto& entry : this->_data.entries) {
      index->insert(entry.first, entry.second);
    }

    // benchmark scenario
    Timer timer{};
    for (auto& entry : this->_data.entries) {
      index->erase(entry.first, entry.second);
    }
    const auto duration = timer.lap();
    return {{{duration, 0}}};
  }
};

}  // namespace imib
