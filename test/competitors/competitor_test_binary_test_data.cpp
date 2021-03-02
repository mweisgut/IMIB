#include <algorithm>
#include <cassert>
#include <tuple>
#include <type_traits>

#include "gtest/gtest.h"

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
#include "data/lookup_generator.hpp"
#include "types.hpp"

struct BookFile {
  inline static const std::string name = "../resources/test_data/books_10K_";
};

struct RandomNumbersFile {
  inline static const std::string name = "../resources/test_data/random_10K_float_convertible_";
};

namespace imib {

template <typename Key, typename Value>
struct TestData {
  Data<Key, Value> data;
  std::vector<EqualityLookup<Key>> equality_lookups;
  std::vector<RangeLookup<Key>> range_lookups;
  SortedVector<Key, Value> validation_index;
};

// clang-format off
using CompetitorTestingTypes = ::testing::Types<
  // BB-Tree only supports uint32_t values
  std::tuple<Art<uint64_t, uint64_t>, uint64_t, uint64_t, BookFile>,
  std::tuple<BTreeGoogleAbseil<uint64_t, uint64_t>, uint64_t, uint64_t, BookFile>,
  std::tuple<BTreeTlx<uint64_t, uint64_t>, uint64_t, uint64_t, BookFile>,
  std::tuple<HashMapRobinHoodFlat<uint64_t, uint64_t>, uint64_t, uint64_t, BookFile>,
  std::tuple<HashMapRobinHoodNode<uint64_t, uint64_t>, uint64_t, uint64_t, BookFile>,
  std::tuple<HashMapTslRobin<uint64_t, uint64_t>, uint64_t, uint64_t, BookFile>,
  std::tuple<HashMapTslSparse<uint64_t, uint64_t>, uint64_t, uint64_t, BookFile>,
  std::tuple<Judy<uint64_t, uint64_t>, uint64_t, uint64_t, BookFile>,
  std::tuple<SimpleVector<uint64_t, uint64_t>, uint64_t, uint64_t, BookFile>,
  std::tuple<SkipList<uint64_t, uint64_t>, uint64_t, uint64_t, BookFile>,
  std::tuple<SortedVector<uint64_t, uint64_t>, uint64_t, uint64_t, BookFile>,
  std::tuple<UnorderedMap<uint64_t, uint64_t>, uint64_t, uint64_t, BookFile>,

  // BB-Tree only supports uint32_t values
  // Judy Array can only be used when both keys and values are of type uint64_t
  std::tuple<Art<uint32_t, uint64_t>, uint32_t, uint64_t, BookFile>,
  std::tuple<BTreeGoogleAbseil<uint32_t, uint64_t>, uint32_t, uint64_t, BookFile>,
  std::tuple<BTreeTlx<uint32_t, uint64_t>, uint32_t, uint64_t, BookFile>,
  std::tuple<HashMapRobinHoodFlat<uint32_t, uint64_t>, uint32_t, uint64_t, BookFile>,
  std::tuple<HashMapRobinHoodNode<uint32_t, uint64_t>, uint32_t, uint64_t, BookFile>,
  std::tuple<HashMapTslRobin<uint32_t, uint64_t>, uint32_t, uint64_t, BookFile>,
  std::tuple<HashMapTslSparse<uint32_t, uint64_t>, uint32_t, uint64_t, BookFile>,
  std::tuple<SimpleVector<uint32_t, uint64_t>, uint32_t, uint64_t, BookFile>,
  std::tuple<SkipList<uint32_t, uint64_t>, uint32_t, uint64_t, BookFile>,
  std::tuple<SortedVector<uint32_t, uint64_t>, uint32_t, uint64_t, BookFile>,
  std::tuple<UnorderedMap<uint32_t, uint64_t>, uint32_t, uint64_t, BookFile>,

  // BB-Tree only uses float keys, thus, converting uint values to float and vise versa can lead to wrong results.
  // Judy Array can only be used when both keys and values are of type uint64_t
  // !!! using TID = uint64_t; is defined in N.h of the ARTSynchronized library. Setting it to
  // TID = uint32_t; allows experiments with the below test
  // std::tuple<Art<uint64_t, uint32_t>, uint64_t, uint32_t>,
  std::tuple<BTreeGoogleAbseil<uint64_t, uint32_t>, uint64_t, uint32_t, BookFile>,
  std::tuple<BTreeTlx<uint64_t, uint32_t>, uint64_t, uint32_t, BookFile>,
  std::tuple<HashMapRobinHoodFlat<uint64_t, uint32_t>, uint64_t, uint32_t, BookFile>,
  std::tuple<HashMapRobinHoodNode<uint64_t, uint32_t>, uint64_t, uint32_t, BookFile>,
  std::tuple<HashMapTslRobin<uint64_t, uint32_t>, uint64_t, uint32_t, BookFile>,
  std::tuple<HashMapTslSparse<uint64_t, uint32_t>, uint64_t, uint32_t, BookFile>,
  std::tuple<SimpleVector<uint64_t, uint32_t>, uint64_t, uint32_t, BookFile>,
  std::tuple<SkipList<uint64_t, uint32_t>, uint64_t, uint32_t, BookFile>,
  std::tuple<SortedVector<uint64_t, uint32_t>, uint64_t, uint32_t, BookFile>,
  std::tuple<UnorderedMap<uint64_t, uint32_t>, uint64_t, uint32_t, BookFile>,

  // BB-Tree only uses float keys, thus, converting uint values to float and vise versa can lead to wrong results.
  // Judy Array can only be used when both keys and values are of type uint64_t
  // !!! using TID = uint64_t; is defined in N.h of the ARTSynchronized library. Setting it to
  // TID = uint32_t; allows experiments with the below test
  // std::tuple<Art<uint32_t, uint32_t>, uint32_t, uint32_t>,
  std::tuple<BTreeGoogleAbseil<uint32_t, uint32_t>, uint32_t, uint32_t, BookFile>,
  std::tuple<BTreeTlx<uint32_t, uint32_t>, uint32_t, uint32_t, BookFile>,
  std::tuple<HashMapRobinHoodFlat<uint32_t, uint32_t>, uint32_t, uint32_t, BookFile>,
  std::tuple<HashMapRobinHoodNode<uint32_t, uint32_t>, uint32_t, uint32_t, BookFile>,
  std::tuple<HashMapTslRobin<uint32_t, uint32_t>, uint32_t, uint32_t, BookFile>,
  std::tuple<HashMapTslSparse<uint32_t, uint32_t>, uint32_t, uint32_t, BookFile>,
  std::tuple<SimpleVector<uint32_t, uint32_t>, uint32_t, uint32_t, BookFile>,
  std::tuple<SkipList<uint32_t, uint32_t>, uint32_t, uint32_t, BookFile>,
  std::tuple<SortedVector<uint32_t, uint32_t>, uint32_t, uint32_t, BookFile>,
  std::tuple<UnorderedMap<uint32_t, uint32_t>, uint32_t, uint32_t, BookFile>,

  // BB-Tree only supports uint32_t values
  std::tuple<Art<uint64_t, uint64_t>, uint64_t, uint64_t, RandomNumbersFile>,
  std::tuple<BTreeGoogleAbseil<uint64_t, uint64_t>, uint64_t, uint64_t, RandomNumbersFile>,
  std::tuple<BTreeTlx<uint64_t, uint64_t>, uint64_t, uint64_t, RandomNumbersFile>,
  std::tuple<HashMapRobinHoodFlat<uint64_t, uint64_t>, uint64_t, uint64_t, RandomNumbersFile>,
  std::tuple<HashMapRobinHoodNode<uint64_t, uint64_t>, uint64_t, uint64_t, RandomNumbersFile>,
  std::tuple<HashMapTslRobin<uint64_t, uint64_t>, uint64_t, uint64_t, RandomNumbersFile>,
  std::tuple<HashMapTslSparse<uint64_t, uint64_t>, uint64_t, uint64_t, RandomNumbersFile>,
  std::tuple<Judy<uint64_t, uint64_t>, uint64_t, uint64_t, RandomNumbersFile>,
  std::tuple<SimpleVector<uint64_t, uint64_t>, uint64_t, uint64_t, RandomNumbersFile>,
  std::tuple<SkipList<uint64_t, uint64_t>, uint64_t, uint64_t, RandomNumbersFile>,
  std::tuple<SortedVector<uint64_t, uint64_t>, uint64_t, uint64_t, RandomNumbersFile>,
  std::tuple<UnorderedMap<uint64_t, uint64_t>, uint64_t, uint64_t, RandomNumbersFile>,

  // BB-Tree only supports uint32_t values
  // Judy Array can only be used when both keys and values are of type uint64_t
  std::tuple<Art<uint32_t, uint64_t>, uint32_t, uint64_t, RandomNumbersFile>,
  std::tuple<BTreeGoogleAbseil<uint32_t, uint64_t>, uint32_t, uint64_t, RandomNumbersFile>,
  std::tuple<BTreeTlx<uint32_t, uint64_t>, uint32_t, uint64_t, RandomNumbersFile>,
  std::tuple<HashMapRobinHoodFlat<uint32_t, uint64_t>, uint32_t, uint64_t, RandomNumbersFile>,
  std::tuple<HashMapRobinHoodNode<uint32_t, uint64_t>, uint32_t, uint64_t, RandomNumbersFile>,
  std::tuple<HashMapTslRobin<uint32_t, uint64_t>, uint32_t, uint64_t, RandomNumbersFile>,
  std::tuple<HashMapTslSparse<uint32_t, uint64_t>, uint32_t, uint64_t, RandomNumbersFile>,
  std::tuple<SimpleVector<uint32_t, uint64_t>, uint32_t, uint64_t, RandomNumbersFile>,
  std::tuple<SkipList<uint32_t, uint64_t>, uint32_t, uint64_t, RandomNumbersFile>,
  std::tuple<SortedVector<uint32_t, uint64_t>, uint32_t, uint64_t, RandomNumbersFile>,
  std::tuple<UnorderedMap<uint32_t, uint64_t>, uint32_t, uint64_t, RandomNumbersFile>,

  // Judy Array can only be used when both keys and values are of type uint64_t
  // !!! using TID = uint64_t; is defined in N.h of the ARTSynchronized library. Setting it to
  // TID = uint32_t; allows experiments with the below test
  // std::tuple<Art<uint64_t, uint32_t>, uint64_t, uint32_t>,
  std::tuple<BbTree<uint64_t, uint32_t>, uint64_t, uint32_t, RandomNumbersFile>,
  std::tuple<BTreeGoogleAbseil<uint64_t, uint32_t>, uint64_t, uint32_t, RandomNumbersFile>,
  std::tuple<BTreeTlx<uint64_t, uint32_t>, uint64_t, uint32_t, RandomNumbersFile>,
  std::tuple<HashMapRobinHoodFlat<uint64_t, uint32_t>, uint64_t, uint32_t, RandomNumbersFile>,
  std::tuple<HashMapRobinHoodNode<uint64_t, uint32_t>, uint64_t, uint32_t, RandomNumbersFile>,
  std::tuple<HashMapTslRobin<uint64_t, uint32_t>, uint64_t, uint32_t, RandomNumbersFile>,
  std::tuple<HashMapTslSparse<uint64_t, uint32_t>, uint64_t, uint32_t, RandomNumbersFile>,
  std::tuple<SimpleVector<uint64_t, uint32_t>, uint64_t, uint32_t, RandomNumbersFile>,
  std::tuple<SkipList<uint64_t, uint32_t>, uint64_t, uint32_t, RandomNumbersFile>,
  std::tuple<SortedVector<uint64_t, uint32_t>, uint64_t, uint32_t, RandomNumbersFile>,
  std::tuple<UnorderedMap<uint64_t, uint32_t>, uint64_t, uint32_t, RandomNumbersFile>,

  // Judy Array can only be used when both keys and values are of type uint64_t
  // !!! using TID = uint64_t; is defined in N.h of the ARTSynchronized library. Setting it to
  // TID = uint32_t; allows experiments with the below test
  // std::tuple<Art<uint32_t, uint32_t>, uint32_t, uint32_t>,
  std::tuple<BbTree<uint32_t, uint32_t>, uint32_t, uint32_t, RandomNumbersFile>,
  std::tuple<BTreeGoogleAbseil<uint32_t, uint32_t>, uint32_t, uint32_t, RandomNumbersFile>,
  std::tuple<BTreeTlx<uint32_t, uint32_t>, uint32_t, uint32_t, RandomNumbersFile>,
  std::tuple<HashMapRobinHoodFlat<uint32_t, uint32_t>, uint32_t, uint32_t, RandomNumbersFile>,
  std::tuple<HashMapRobinHoodNode<uint32_t, uint32_t>, uint32_t, uint32_t, RandomNumbersFile>,
  std::tuple<HashMapTslRobin<uint32_t, uint32_t>, uint32_t, uint32_t, RandomNumbersFile>,
  std::tuple<HashMapTslSparse<uint32_t, uint32_t>, uint32_t, uint32_t, RandomNumbersFile>,
  std::tuple<SimpleVector<uint32_t, uint32_t>, uint32_t, uint32_t, RandomNumbersFile>,
  std::tuple<SkipList<uint32_t, uint32_t>, uint32_t, uint32_t, RandomNumbersFile>,
  std::tuple<SortedVector<uint32_t, uint32_t>, uint32_t, uint32_t, RandomNumbersFile>,
  std::tuple<UnorderedMap<uint32_t, uint32_t>, uint32_t, uint32_t, RandomNumbersFile>
>;
// clang-format on

template <typename T>
class CompetitorTestBinaryTestData : public ::testing::Test {
  using Index = typename std::tuple_element<0, T>::type;
  using Key = typename std::tuple_element<1, T>::type;
  using Value = typename std::tuple_element<2, T>::type;

 protected:
  void SetUp() override {
    // do nothing
  }

  void TearDown() override {
    // do nothing
  }

  TestData<Key, Value> setup_test_data(const std::string& file_name_without_type) {
    const auto type_name = Type<Key>::name();
    const auto suffix = type_name.substr(0, type_name.size() - 2);
    const auto data_filename = file_name_without_type + suffix;
    const auto data = DataGenerator::generate<Key, Value>(data_filename);
    const auto equality_lookups = LookupGenerator::generate_equality_lookups<Key>(data_filename + "_equality_lookups");
    const auto range_lookups = LookupGenerator::generate_range_lookups<Key>(data_filename + "_range_lookups");
    SortedVector<Key, Value> validation_index;
    validation_index.bulk_insert(data);
    // fill the column used by the ART index
    Column<Key, Value>::fill(data.entries);
    return TestData<Key, Value>{std::move(data), std::move(equality_lookups), std::move(range_lookups),
                                std::move(validation_index)};
  }
};

TYPED_TEST_SUITE(CompetitorTestBinaryTestData, CompetitorTestingTypes);

// Test cases are written with the assumption that all competitors support at least the IndexOperationTypes
// InsertEntry, EraseEntry and EqualityLookup

TYPED_TEST(CompetitorTestBinaryTestData, BulkLoadEqualityLookup) {
  using Index = typename std::tuple_element<0, TypeParam>::type;
  using Key = typename std::tuple_element<1, TypeParam>::type;
  using Value = typename std::tuple_element<2, TypeParam>::type;
  using File = typename std::tuple_element<3, TypeParam>::type;
  const auto test_data = this->setup_test_data(File::name);
  Index index;
  if (!Index::supports(IndexOperationType::BulkLoad)) {
    EXPECT_THROW(index.bulk_load(test_data.data), std::logic_error);
    return;
  }

  if (Index::type() == IndexType::BTreeTLX) {
    // the bulk load operation of the BTreeTLX requires sorted entries
    auto entries = test_data.data.entries;
    std::sort(entries.begin(), entries.end());
    index.bulk_load(Data<Key, Value>{{}, {}, std::move(entries), {}});
  } else {
    index.bulk_load(test_data.data);
  }

  for (const auto& entry : test_data.data.entries) {
    const auto actual_result = index.equality_lookup(entry.first);
    EXPECT_EQ(actual_result, std::vector<Value>{entry.second});
  }
}

TYPED_TEST(CompetitorTestBinaryTestData, BulkInsertEqualityLookup) {
  using Index = typename std::tuple_element<0, TypeParam>::type;
  using Key = typename std::tuple_element<1, TypeParam>::type;
  using Value = typename std::tuple_element<2, TypeParam>::type;
  using File = typename std::tuple_element<3, TypeParam>::type;
  const auto test_data = this->setup_test_data(File::name);
  Index index;
  if (!Index::supports(IndexOperationType::BulkInsert)) {
    EXPECT_THROW(index.bulk_insert(test_data.data), std::logic_error);
    return;
  }

  index.bulk_insert(test_data.data);

  for (const auto& entry : test_data.data.entries) {
    const auto actual_result = index.equality_lookup(entry.first);
    EXPECT_EQ(actual_result, std::vector<Value>{entry.second});
  }
}

TYPED_TEST(CompetitorTestBinaryTestData, InsertEqualityLookup) {
  using Index = typename std::tuple_element<0, TypeParam>::type;
  using File = typename std::tuple_element<3, TypeParam>::type;
  const auto test_data = this->setup_test_data(File::name);
  Index index;
  // store all entries
  for (const auto& entry : test_data.data.entries) {
    index.insert(entry.first, entry.second);
  }
  // perform lookups
  for (const auto& lookup : test_data.equality_lookups) {
    const auto actual_result = index.equality_lookup(lookup.key);
    const auto expected_result = test_data.validation_index.equality_lookup(lookup.key);
    EXPECT_EQ(actual_result, expected_result);
  }
}

TYPED_TEST(CompetitorTestBinaryTestData, InsertRangeLookup) {
  using Index = typename std::tuple_element<0, TypeParam>::type;
  using Key = typename std::tuple_element<1, TypeParam>::type;
  using File = typename std::tuple_element<3, TypeParam>::type;

  // store all entries
  const auto test_data = this->setup_test_data(File::name);
  Index index;
  for (const auto& entry : test_data.data.entries) {
    {
      const auto result = index.equality_lookup(entry.first);
      EXPECT_TRUE(result.empty());
    }
    index.insert(entry.first, entry.second);
    {
      const auto result = index.equality_lookup(entry.first);
      EXPECT_EQ(result.size(), 1u);
      EXPECT_EQ(result[0], entry.second);
    }
  }

  if (!Index::supports(IndexOperationType::RangeLookup)) {
    EXPECT_THROW(index.range_lookup(Key{}, Key{}), std::logic_error);
    return;
  }

  for (const auto& lookup : test_data.range_lookups) {
    auto actual_result = index.range_lookup(lookup.lower_bound_key, lookup.upper_bound_key);
    auto expected_result = test_data.validation_index.range_lookup(lookup.lower_bound_key, lookup.upper_bound_key);
    std::sort(actual_result.begin(), actual_result.end());
    std::sort(expected_result.begin(), expected_result.end());
    EXPECT_EQ(actual_result.size(), expected_result.size());
    EXPECT_EQ(actual_result, expected_result);
  }
}

TYPED_TEST(CompetitorTestBinaryTestData, InsertEqualityLookupErase) {
  using Index = typename std::tuple_element<0, TypeParam>::type;
  using Key = typename std::tuple_element<1, TypeParam>::type;
  using Value = typename std::tuple_element<2, TypeParam>::type;
  using File = typename std::tuple_element<3, TypeParam>::type;

  // store all entries
  const auto test_data = this->setup_test_data(File::name);
  Index index;
  for (const auto& entry : test_data.data.entries) {
    index.insert(entry.first, entry.second);
  }

  for (const auto& entry : test_data.data.entries) {
    const auto lookup_result_before_erase = index.equality_lookup(entry.first);
    EXPECT_EQ(lookup_result_before_erase, std::vector<Value>{entry.second});
    index.erase(entry.first, entry.second);
    const auto lookup_result_after_erase = index.equality_lookup(entry.first);
    EXPECT_TRUE(lookup_result_after_erase.empty());
  }
}

}  // namespace imib
