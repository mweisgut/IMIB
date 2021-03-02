#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <random>
#include <vector>

#include "types.hpp"

namespace imib {

// c++ std lib distributions: https://en.cppreference.com/w/cpp/numeric/random

enum class DistributionType { Normal, Uniform };

class RandomKeyGenerator {
 public:
  RandomKeyGenerator() = delete;
  template <typename Key>
  static std::vector<Key> generate(const DistributionType distribution_type, const uint64_t entry_count,
                                   const bool unique_keys_required);
};

}  // namespace imib
