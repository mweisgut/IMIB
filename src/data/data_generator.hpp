#pragma once

#include <vector>

#include "entry_generator.hpp"
#include "key_generator.hpp"
#include "lookup_generator.hpp"
#include "random_key_generator.hpp"
#include "tid_generator.hpp"
#include "types.hpp"

namespace imib {

class DataGenerator {
 public:
  DataGenerator() = delete;

  template <typename Key, typename Value>
  static Data<Key, Value> generate(const std::string& file_path) {
    const auto keys = KeyGenerator::generate<Key>(file_path);
    const auto values = TIDGenerator::generate<Key, Value>(keys);
    const auto entries = EntryGenerator::generate<Key, Value>(keys);
    // BBTree required float keys (const std::vector<std::vector<float>>) for the bulk load operation
    const auto float_keys = KeyGenerator::generate_float<Key>(keys);
    const auto data = Data(std::move(keys), std::move(values), std::move(entries), std::move(float_keys));
    return data;
  }
};

}  // namespace imib
