#pragma once

#include <stdexcept>
#include <type_traits>

#include "abstract_benchmark_case.hpp"
#include "competitors/simple_vector.hpp"
#include "evaluation/memory_tracker.hpp"
#include "evaluation/timer.hpp"
#include "types.hpp"

namespace imib {

template <typename Index, typename Key, typename Value>
class CaseEqualityLookup final : public AbstractBenchmarkCase<Index, Key, Value> {
 public:
  explicit CaseEqualityLookup(const Data<Key, Value>& data, const std::vector<EqualityLookup<Key>>& equality_lookups)
      : AbstractBenchmarkCase<Index, Key, Value>(data), _equality_lookups(equality_lookups) {
    // do nothing;
  }

  const std::string name() const override { return "EqualityLookup"; }

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
    for (const auto& lookup : this->_equality_lookups) {
      index->equality_lookup(lookup.key);
    }
    const auto duration = timer.lap();
    return {{{duration, 0}}};
  }

 private:
  const std::vector<EqualityLookup<Key>>& _equality_lookups;
};

}  // namespace imib
