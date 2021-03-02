#pragma once

#include <stdexcept>

#include "abstract_benchmark_case.hpp"
#include "competitors/simple_vector.hpp"
#include "evaluation/memory_tracker.hpp"
#include "evaluation/timer.hpp"
#include "types.hpp"

namespace imib {

template <typename Index, typename Key, typename Value>
class CaseRangeLookup final : public AbstractBenchmarkCase<Index, Key, Value> {
 public:
  explicit CaseRangeLookup(const Data<Key, Value>& data, const std::vector<RangeLookup<Key>>& range_lookups)
      : AbstractBenchmarkCase<Index, Key, Value>(data), _range_lookups(range_lookups) {
    // do nothing;
  }

  const std::string name() const override { return "RangeLookup"; }

  // only time is tracked, memory is set to 0
  const ExecutionStatistics execute() const override {
    // preparation: fill index
    auto index = std::make_shared<Index>();
    if (Index::type() == IndexType::SortedVector) {
      index->bulk_insert(this->_data);
    } else {
      for (auto& entry : this->_data.entries) {
        index->insert(entry.first, entry.second);
      }
    }

    // benchmark scenario: execute x equality lookups
    Timer timer{};
    for (const auto& lookup_range : this->_range_lookups) {
      index->range_lookup(lookup_range.lower_bound_key, lookup_range.upper_bound_key);
    }
    const auto duration = timer.lap();
    return {{{duration, 0}}};
  }

 private:
  const std::vector<RangeLookup<Key>>& _range_lookups;
};

}  // namespace imib
