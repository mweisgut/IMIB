#pragma once

#include <algorithm>
#include <vector>

#include "load_binary_data.hpp"
#include "types.hpp"

namespace imib {

class LookupGenerator {
 public:
  LookupGenerator() = delete;

  template <typename Key>
  static std::vector<EqualityLookup<Key>> generate_equality_lookups(const std::string& filename) {
    auto keys = load_binary_data<Key>(filename);
    auto lookups = std::vector<EqualityLookup<Key>>{};
    lookups.reserve(keys.size());
    for (auto& key : keys) {
      lookups.emplace_back(key);
    }
    return lookups;
  }

  template <typename Key>
  static std::vector<RangeLookup<Key>> generate_range_lookups(const std::string& filename) {
    // every two consecutive values form a pair of values that defines a range.
    // the smaller value is the lower bound value, the larger one the upper bound value.
    auto keys = load_binary_data<Key>(filename);
    if (keys.size() % 2 > 0) {
      throw std::logic_error(
          "Cannot build ranges: an even number of values is required. Two consecutive values form a value pair, i.e., "
          "a range.");
    }
    auto lookups = std::vector<RangeLookup<Key>>{};
    lookups.reserve(keys.size() / 2);
    for (size_t index = 0; index < keys.size(); index += 2) {
      const auto& lower_bound = std::min(keys[index], keys[index + 1]);
      const auto& upper_bound = std::max(keys[index], keys[index + 1]);
      lookups.emplace_back(lower_bound, upper_bound);
    }
    return lookups;
  }
};

}  // namespace imib
