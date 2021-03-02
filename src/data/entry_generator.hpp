#pragma once

#include <vector>

#include "types.hpp"

namespace imib {

class EntryGenerator {
 public:
  EntryGenerator() = delete;

  template <typename Key, typename Value>
  static std::vector<std::pair<Key, Value>> generate(const std::vector<Key>& keys) {
    std::vector<std::pair<Key, Value>> entries(keys.size());
    static_assert(std::is_same_v<Value, uint32_t> || std::is_same_v<Value, uint64_t>,
                  "Only uint32_t or uint64_t values are supported so far.");
    // the value (i.e., tuple id on a key) is its position, starting from 1.
    for (Value position = 0; position < keys.size(); ++position) {
      entries[position] = {keys[position], position + 1};
    }

    return entries;
  }
};

}  // namespace imib
