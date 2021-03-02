#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_set>

#include "competitors/art.hpp"
#include "competitors/b_tree_google_abseil.hpp"
#include "competitors/b_tree_tlx.hpp"
#include "competitors/bb_tree.hpp"
#include "competitors/hash_map_robin_hood_flat.hpp"
#include "competitors/hash_map_robin_hood_node.hpp"
#include "competitors/hash_map_tsl_robin.hpp"
#include "competitors/hash_map_tsl_sparse.hpp"
#include "competitors/judy.hpp"
#include "competitors/simple_vector.hpp"
#include "competitors/skip_list.hpp"
#include "competitors/sorted_vector.hpp"
#include "competitors/unordered_map.hpp"
#include "data/column.hpp"
#include "data/data_generator.hpp"
#include "execution/benchmark_runner.hpp"
#include "execution/cases/case_bulk_insert.hpp"
#include "execution/cases/case_bulk_load.hpp"
#include "execution/cases/case_equality_lookup.hpp"
#include "execution/cases/case_erase.hpp"
#include "execution/cases/case_insert.hpp"
#include "execution/cases/case_range_lookup.hpp"
#include "types.hpp"

namespace imib {

std::string now_as_string() {
  auto now = std::time(nullptr);
  auto local_now = *std::localtime(&now);

  std::ostringstream oss;
  oss << std::put_time(&local_now, "%Y-%m-%d_%H-%M-%S");
  return oss.str();
}

template <typename K, typename V>
void print_data(const std::string& data_file, const std::string& eq_lookup_file, const std::string& range_lookup_file) {
  const auto data = DataGenerator::generate<K, V>(data_file);
  const auto equality_lookups = LookupGenerator::generate_equality_lookups<K>(eq_lookup_file);
  const auto range_lookups = LookupGenerator::generate_range_lookups<K>(range_lookup_file);
  for (size_t index = 0; index < data.keys.size(); ++index) {
    std::cout << "K: " << data.keys[index] << "|K f1d: " << data.keys_float[index][0] << "|V: " << data.values[index]
              << '\n';
  }

  for (const auto& entry : data.entries) {
    std::cout << entry.first << ", " << entry.second << '\n';
  }

  std::cout << "print eq lookups" << std::endl;
  for (const auto& lookup : equality_lookups) {
    std::cout << lookup.key << '\n';
  }

  std::cout << "print range lookups" << std::endl;
  for (const auto& lookup : range_lookups) {
    std::cout << lookup.lower_bound_key << ", " << lookup.upper_bound_key << '\n';
  }
}

template <typename K, typename V>
void setup_and_run_benchmark(const BenchmarkConfiguration config) {
  const auto data = DataGenerator::generate<K, V>(config.data_file);
  const auto equality_lookups = LookupGenerator::generate_equality_lookups<K>(config.equality_lookup_file);
  const auto range_lookups = LookupGenerator::generate_range_lookups<K>(config.range_lookup_file);

  // The ART internally reads the value by a given TID. Thus, we have to fill a column that is used by the ART.
  Column<K, V>::fill(data.entries);
  auto cases = std::vector<std::shared_ptr<BaseBenchmarkCase>>{};
  cases.reserve(65);

  // TODO(Marcel) do the data generation and benchmark case creation within the benchmark runner. Just pass
  // configuration records for the benchmark cases: CaseType, IndexType, KeyType, ValueType, for example:
  // {BulkLoad, BTreeTlx, uint32_t, uint32_t}

  // #########
  // BULK LOAD
  // #########
  // The BBTree stores values as a vector of floats. Thus, we cast the keys to float. Use the BBTree with caution
  // and only when the uint32 values can be cast to float values without narrowing overflow.
  // if constexpr (std::is_same_v<V, uint32_t>) {
  //   cases.push_back(std::make_shared<CaseBulkLoad<BbTree<K, V>, K, V>>(data));
  // }
  cases.push_back(std::make_shared<CaseBulkLoad<BTreeTlx<K, V>, K, V>>(data));
  // ###########
  // BULK INSERT
  // ###########
  cases.push_back(std::make_shared<CaseBulkInsert<BTreeGoogleAbseil<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseBulkInsert<BTreeTlx<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseBulkInsert<HashMapRobinHoodFlat<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseBulkInsert<HashMapRobinHoodNode<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseBulkInsert<HashMapTslRobin<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseBulkInsert<HashMapTslSparse<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseBulkInsert<SimpleVector<K, V>, K, V>>(data));
  // measurements on small data sets show that the SkipList is very slow.
  // cases.push_back(std::make_shared<CaseBulkInsert<SkipList<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseBulkInsert<SortedVector<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseBulkInsert<UnorderedMap<K, V>, K, V>>(data));
  // ###############
  // EQUALITY LOOKUP
  // ###############
  cases.push_back(std::make_shared<CaseEqualityLookup<Art<K, V>, K, V>>(data, equality_lookups));
  // The BBTree stores values as a vector of floats. Thus, we cast the keys to float. Use the BBTree with caution
  // and only when the uint32 values can be cast to float values without narrowing overflow.
  // if constexpr (std::is_same_v<V, uint32_t>) {
  //   cases.push_back(std::make_shared<CaseEqualityLookup<BbTree<K, V>, K, V>>(data, equality_lookups));
  // }
  cases.push_back(std::make_shared<CaseEqualityLookup<BTreeGoogleAbseil<K, V>, K, V>>(data, equality_lookups));
  cases.push_back(std::make_shared<CaseEqualityLookup<BTreeTlx<K, V>, K, V>>(data, equality_lookups));
  cases.push_back(std::make_shared<CaseEqualityLookup<HashMapRobinHoodFlat<K, V>, K, V>>(data, equality_lookups));
  cases.push_back(std::make_shared<CaseEqualityLookup<HashMapRobinHoodNode<K, V>, K, V>>(data, equality_lookups));
  cases.push_back(std::make_shared<CaseEqualityLookup<HashMapTslRobin<K, V>, K, V>>(data, equality_lookups));
  cases.push_back(std::make_shared<CaseEqualityLookup<HashMapTslSparse<K, V>, K, V>>(data, equality_lookups));
  if constexpr (std::is_same_v<V, uint64_t> && std::is_same_v<K, uint64_t>) {
    cases.push_back(std::make_shared<CaseEqualityLookup<Judy<K, V>, K, V>>(data, equality_lookups));
  }
  // takes an incredibly long time
  // cases.push_back(std::make_shared<CaseEqualityLookup<SimpleVector<K, V>, K, V>>(data, equality_lookups));
  // measurements on small data sets show that the SkipList is very slow.
  // cases.push_back(std::make_shared<CaseEqualityLookup<SkipList<K, V>, K, V>>(data, equality_lookups));
  cases.push_back(std::make_shared<CaseEqualityLookup<SortedVector<K, V>, K, V>>(data, equality_lookups));
  cases.push_back(std::make_shared<CaseEqualityLookup<UnorderedMap<K, V>, K, V>>(data, equality_lookups));
  // ############
  // RANGE LOOKUP
  // ############
  // based on the competitor binary tests, BBTree's range lookup is faulty
  // if constexpr (std::is_same_v<V, uint32_t>) {
  //   cases.push_back(std::make_shared<CaseRangeLookup<BbTree<K, V>, K, V>>(data, range_lookups));
  // }
  cases.push_back(std::make_shared<CaseRangeLookup<BTreeGoogleAbseil<K, V>, K, V>>(data, range_lookups));
  cases.push_back(std::make_shared<CaseRangeLookup<BTreeTlx<K, V>, K, V>>(data, range_lookups));
  // measurements on small data sets show that the SkipList is very slow.
  // cases.push_back(std::make_shared<CaseRangeLookup<SkipList<K, V>, K, V>>(data, range_lookups));
  cases.push_back(std::make_shared<CaseRangeLookup<SortedVector<K, V>, K, V>>(data, range_lookups));
  // ######
  // INSERT
  // ######
  cases.push_back(std::make_shared<CaseInsert<Art<K, V>, K, V>>(data));
  // The BBTree stores values as a vector of floats. Thus, we cast the keys to float. Use the BBTree with caution
  // and only when the uint32 values can be cast to float values without narrowing overflow.
  // if constexpr (std::is_same_v<V, uint32_t>) {
  //   cases.push_back(std::make_shared<CaseInsert<BbTree<K, V>, K, V>>(data));
  // }
  cases.push_back(std::make_shared<CaseInsert<BTreeGoogleAbseil<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseInsert<BTreeTlx<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseInsert<HashMapRobinHoodFlat<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseInsert<HashMapRobinHoodNode<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseInsert<HashMapTslRobin<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseInsert<HashMapTslSparse<K, V>, K, V>>(data));
  if constexpr (std::is_same_v<V, uint64_t> && std::is_same_v<K, uint64_t>) {
    cases.push_back(std::make_shared<CaseInsert<Judy<K, V>, K, V>>(data));
  }
  cases.push_back(std::make_shared<CaseInsert<SimpleVector<K, V>, K, V>>(data));
  // measurements on small data sets show that the SkipList is very slow.
  // cases.push_back(std::make_shared<CaseInsert<SkipList<K, V>, K, V>>(data));
  // takes an incredibly long time
  // cases.push_back(std::make_shared<CaseInsert<SortedVector<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseInsert<UnorderedMap<K, V>, K, V>>(data));
  // #####
  // ERASE
  // #####
  // based on the competitor binary tests, ART's range lookup is faulty
  // cases.push_back(std::make_shared<CaseErase<Art<K, V>, K, V>>(data));
  // The BBTree stores values as a vector of floats. Thus, we cast the keys to float. Use the BBTree with caution
  // and only when the uint32 values can be cast to float values without narrowing overflow.
  // if constexpr (std::is_same_v<V, uint32_t>) {
  //   cases.push_back(std::make_shared<CaseErase<BbTree<K, V>, K, V>>(data));
  // }
  cases.push_back(std::make_shared<CaseErase<BTreeGoogleAbseil<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseErase<BTreeTlx<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseErase<HashMapRobinHoodFlat<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseErase<HashMapRobinHoodNode<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseErase<HashMapTslRobin<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseErase<HashMapTslSparse<K, V>, K, V>>(data));
  if constexpr (std::is_same_v<V, uint64_t> && std::is_same_v<K, uint64_t>) {
    cases.push_back(std::make_shared<CaseErase<Judy<K, V>, K, V>>(data));
  }
  // takes an incredibly long time
  // cases.push_back(std::make_shared<CaseErase<SimpleVector<K, V>, K, V>>(data));
  // measurements on small data sets show that the SkipList is very slow.
  // cases.push_back(std::make_shared<CaseErase<SkipList<K, V>, K, V>>(data));
  // takes an incredibly long time
  // cases.push_back(std::make_shared<CaseErase<SortedVector<K, V>, K, V>>(data));
  cases.push_back(std::make_shared<CaseErase<UnorderedMap<K, V>, K, V>>(data));

  auto benchmark_runner = BenchmarkRunner(cases, config);
  benchmark_runner.run();
}

}  // namespace imib

int main(int argc, char* argv[]) {
  if (argc != 7) {
    std::cerr << "Usage: imiBench <key_type> <iterations> <data binary file> <equality lookup file> <range lookup "
                 "file> <result "
                 "file prefix (w/o extension)>"
              << std::endl;
    return 1;
  }

  using Value = uint64_t;
  const auto supported_types = std::unordered_set<std::string>{"uint32_t", "uint64_t"};
  const std::string key_type{argv[1]};
  if (!supported_types.contains(key_type)) {
    std::cerr << "Key type '" << key_type << "' is not supported.\n";
    return 1;
  }
  const auto iterations{std::stoul(std::string{argv[2]})};
  const std::string data_file{argv[3]};
  const std::string equality_lookup_file{argv[4]};
  const std::string range_lookup_file{argv[5]};
  const std::string result_file_prefix{argv[6]};
  const std::string result_file_name = result_file_prefix + "_" + imib::now_as_string() + ".json";

  std::cout << "Benchmark Configurations:\n"
            << "  Key type:               " << key_type << '\n'
            << "  Value type:             " << imib::Type<Value>::name() << '\n'
            << "  Number of iterations:   " << iterations << '\n'
            << "  Data file:              " << data_file << '\n'
            << "  Equality lookup file:   " << equality_lookup_file << '\n'
            << "  Range lookup file:      " << range_lookup_file << '\n'
            << "  Result file:            " << result_file_name << "\n\n";
  const auto config =
      imib::BenchmarkConfiguration{iterations,  // number of iterations
                                   1,           // number of threads
                                   data_file,  equality_lookup_file, range_lookup_file, result_file_name};
  // TODO(Marcel): Switch case with KeyTypes would be more elegant.
  if (key_type == "uint32_t") {
    imib::setup_and_run_benchmark<uint32_t, Value>(std::move(config));
  } else if (key_type == "uint64_t") {
    imib::setup_and_run_benchmark<uint64_t, Value>(std::move(config));
  }
  std::cout << "Finished benchmark execution." << std::endl;
}
