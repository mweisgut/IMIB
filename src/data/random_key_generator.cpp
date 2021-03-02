#include "random_key_generator.hpp"

#include <algorithm>
#include <functional>
#include <limits>
#include <random>
#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace imib {

template <typename Key>
std::vector<Key> RandomKeyGenerator::generate(const DistributionType distribution_type, const uint64_t entry_count,
                                              const bool unique_keys_required) {
  // setup value generation function and data name
  std::function<Key()> generate_value = nullptr;
  std::string data_name{};

  std::random_device random_device;
  std::mt19937 random_engine(random_device());

  switch (distribution_type) {
    case DistributionType::Normal:
      throw std::logic_error("Passed data generation strategy is not supported.");
    case DistributionType::Uniform:
      static_assert(std::is_same_v<Key, int32_t> || std::is_same_v<Key, int64_t>,
                    "Only int32_t or int64_t keys are supported.");
      std::uniform_int_distribution<Key> uniform_int_distribution(1, 50);
      generate_value = [&]() { return uniform_int_distribution(random_engine); };
      data_name = "Uniform";
      break;
  }

  // generate keys
  std::vector<Key> keys(entry_count);

  if (unique_keys_required) {
    // generate unique keys
    std::unordered_set<Key> unique_keys{};
    unique_keys.reserve(entry_count);

    while (unique_keys.size() < entry_count) {
      unique_keys.insert(generate_value());
    }

    auto iter = unique_keys.begin();
    auto position = 0u;
    while (iter != unique_keys.end()) {
      keys[position] = std::move(unique_keys.extract(iter++).value());
      ++position;
    }
  } else {
    // generate keys that can contain duplicates
    for (uint64_t position = 0; position < entry_count; ++position) {
      auto value = generate_value();
      keys[position] = value;
    }
  }

  return keys;
  // TODO(Marcel) create a better technique for generating the lookup lists
  // generate equality lookups
  // std::vector<Key> equality_lookups{};
  // equality_lookups.reserve(_equality_lookup_count);

  // std::sample(keys.begin(), keys.end(), std::back_inserter(equality_lookups), _equality_lookup_count,
  //             std::mt19937{std::random_device{}()});

  // // generate range lookups
  // std::vector<Key> flattened_range_lookups{};
  // flattened_range_lookups.reserve(2 * _range_lookup_count);

  // std::sample(keys.begin(), keys.end(), std::back_inserter(flattened_range_lookups), 2 * _range_lookup_count,
  //             std::mt19937{std::random_device{}()});

  // std::vector<std::pair<Key, Key>> range_lookups{};
  // range_lookups.reserve(_range_lookup_count);

  // for (auto index = 0u; index < flattened_range_lookups.size(); ++index) {
  //   if (index % 2 == 0) {
  //     continue;
  //   }
  //   // for each odd index
  //   const auto& first = flattened_range_lookups[index - 1];
  //   const auto& second = flattened_range_lookups[index];
  //   range_lookups.emplace_back(std::min(first, second), std::max(first, second));
  // }

  // auto data = std::make_shared<Data<Key, Value>>(data_name, std::move(entries), unique_keys_required,
  // equality_lookups, range_lookups);
  // return data;
}

template std::vector<int32_t> RandomKeyGenerator::generate<int32_t>(const DistributionType distribution_type,
                                                                    const uint64_t entry_count,
                                                                    const bool unique_keys_required);

template std::vector<int64_t> RandomKeyGenerator::generate<int64_t>(const DistributionType distribution_type,
                                                                    const uint64_t entry_count,
                                                                    const bool unique_keys_required);

}  // namespace imib
