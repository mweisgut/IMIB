#pragma once

#include <algorithm>
#include <vector>

#include "types.hpp"

namespace imib {

class TIDGenerator {
 public:
  TIDGenerator() = delete;

  template <typename Key, typename TID>
  static std::vector<TID> generate(const std::vector<Key>& keys) {
    std::vector<TID> tids(keys.size());
    std::iota(tids.begin(), tids.end(), 1);
    return tids;
  }
};

}  // namespace imib
