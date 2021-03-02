#pragma once

#include "load_binary_data.hpp"

namespace imib {

class KeyGenerator {
 public:
  KeyGenerator() = delete;

  template <typename Key>
  static std::vector<Key> generate(const std::string& filename) {
    return load_binary_data<Key>(filename);
  }

  template <typename Key>
  static std::vector<std::vector<float>> generate_float(const std::vector<Key> keys) {
    std::vector<std::vector<float>> float_keys(keys.size(), std::vector<float>{.0f});
    for (size_t index = 0; index < keys.size(); ++index) {
      // narrowing issue! Key could exceed float max values
      float_keys[index][0] = static_cast<float>(keys[index]);
    }
    return float_keys;
  }
};

}  // namespace imib
