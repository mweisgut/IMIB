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
#include "data/key_generator.hpp"
#include "types.hpp"

namespace imib {

// clang-format off
using CompetitorTestingTypes = ::testing::Types<
  // ########## uint64 keys, uint64 values
  std::tuple<Art<uint64_t, uint64_t>, uint64_t, uint64_t>,
  // BB-Tree only supports uint32_t values
  // std::tuple<BbTree<uint64_t, uint64_t>, uint64_t, uint64_t>,
  std::tuple<BTreeGoogleAbseil<uint64_t, uint64_t>, uint64_t, uint64_t>,
  std::tuple<BTreeTlx<uint64_t, uint64_t>, uint64_t, uint64_t>,
  std::tuple<HashMapRobinHoodFlat<uint64_t, uint64_t>, uint64_t, uint64_t>,
  std::tuple<HashMapRobinHoodNode<uint64_t, uint64_t>, uint64_t, uint64_t>,
  std::tuple<HashMapTslRobin<uint64_t, uint64_t>, uint64_t, uint64_t>,
  std::tuple<HashMapTslSparse<uint64_t, uint64_t>, uint64_t, uint64_t>,
  std::tuple<Judy<uint64_t, uint64_t>, uint64_t, uint64_t>,
  std::tuple<SimpleVector<uint64_t, uint64_t>, uint64_t, uint64_t>,
  std::tuple<SkipList<uint64_t, uint64_t>, uint64_t, uint64_t>,
  std::tuple<SortedVector<uint64_t, uint64_t>, uint64_t, uint64_t>,
  std::tuple<UnorderedMap<uint64_t, uint64_t>, uint64_t, uint64_t>,
  // ########## uint32 keys, uint64 values
  std::tuple<Art<uint32_t, uint64_t>, uint32_t, uint64_t>,
  // BB-Tree only supports uint32_t values
  // std::tuple<BbTree<uint32_t, uint64_t>, uint32_t, uint64_t>,
  std::tuple<BTreeGoogleAbseil<uint32_t, uint64_t>, uint32_t, uint64_t>,
  std::tuple<BTreeTlx<uint32_t, uint64_t>, uint32_t, uint64_t>,
  std::tuple<HashMapRobinHoodFlat<uint32_t, uint64_t>, uint32_t, uint64_t>,
  std::tuple<HashMapRobinHoodNode<uint32_t, uint64_t>, uint32_t, uint64_t>,
  std::tuple<HashMapTslRobin<uint32_t, uint64_t>, uint32_t, uint64_t>,
  std::tuple<HashMapTslSparse<uint32_t, uint64_t>, uint32_t, uint64_t>,
  // Following test fails: Both template parameters must be the same size as a void*
  // std::tuple<Judy<uint32_t, uint64_t>, uint32_t, uint64_t>,
  std::tuple<SimpleVector<uint32_t, uint64_t>, uint32_t, uint64_t>,
  std::tuple<SkipList<uint32_t, uint64_t>, uint32_t, uint64_t>,
  std::tuple<SortedVector<uint32_t, uint64_t>, uint32_t, uint64_t>,
  std::tuple<UnorderedMap<uint32_t, uint64_t>, uint32_t, uint64_t>,
  // ########## uint64 keys, uint32 values
  // !!! using TID = uint64_t; is defined in N.h of the ARTSynchronized library. Setting it to
  // TID = uint32_t; allows experiments with the below test
  // std::tuple<Art<uint64_t, uint32_t>, uint64_t, uint32_t>,
  std::tuple<BbTree<uint64_t, uint32_t>, uint64_t, uint32_t>,
  std::tuple<BTreeGoogleAbseil<uint64_t, uint32_t>, uint64_t, uint32_t>,
  std::tuple<BTreeTlx<uint64_t, uint32_t>, uint64_t, uint32_t>,
  std::tuple<HashMapRobinHoodFlat<uint64_t, uint32_t>, uint64_t, uint32_t>,
  std::tuple<HashMapRobinHoodNode<uint64_t, uint32_t>, uint64_t, uint32_t>,
  std::tuple<HashMapTslRobin<uint64_t, uint32_t>, uint64_t, uint32_t>,
  std::tuple<HashMapTslSparse<uint64_t, uint32_t>, uint64_t, uint32_t>,
  // Following test fails: Both template parameters must be the same size as a void*
  // std::tuple<Judy<uint64_t, uint32_t>, uint64_t, uint32_t>,
  std::tuple<SimpleVector<uint64_t, uint32_t>, uint64_t, uint32_t>,
  std::tuple<SkipList<uint64_t, uint32_t>, uint64_t, uint32_t>,
  std::tuple<SortedVector<uint64_t, uint32_t>, uint64_t, uint32_t>,
  std::tuple<UnorderedMap<uint64_t, uint32_t>, uint64_t, uint32_t>,
  // ########## uint32 keys, uint32 values
  // !!! using TID = uint64_t; is defined in N.h of the ARTSynchronized library. Setting it to
  // TID = uint32_t; allows experiments with the below test
  // std::tuple<Art<uint32_t, uint32_t>, uint32_t, uint32_t>,
  std::tuple<BbTree<uint32_t, uint32_t>, uint32_t, uint32_t>,
  std::tuple<BTreeGoogleAbseil<uint32_t, uint32_t>, uint32_t, uint32_t>,
  std::tuple<BTreeTlx<uint32_t, uint32_t>, uint32_t, uint32_t>,
  std::tuple<HashMapRobinHoodFlat<uint32_t, uint32_t>, uint32_t, uint32_t>,
  std::tuple<HashMapRobinHoodNode<uint32_t, uint32_t>, uint32_t, uint32_t>,
  std::tuple<HashMapTslRobin<uint32_t, uint32_t>, uint32_t, uint32_t>,
  std::tuple<HashMapTslSparse<uint32_t, uint32_t>, uint32_t, uint32_t>,
  // Following test fails: Both template parameters must be the same size as a void*
  // std::tuple<Judy<uint32_t, uint32_t>, uint32_t, uint32_t>,
  std::tuple<SimpleVector<uint32_t, uint32_t>, uint32_t, uint32_t>,
  std::tuple<SkipList<uint32_t, uint32_t>, uint32_t, uint32_t>,
  std::tuple<SortedVector<uint32_t, uint32_t>, uint32_t, uint32_t>,
  std::tuple<UnorderedMap<uint32_t, uint32_t>, uint32_t, uint32_t>
>;
// clang-format on

template <typename T>
class CompetitorUniqueKeysTest : public ::testing::Test {
  using Index = typename std::tuple_element<0, T>::type;
  using Key = typename std::tuple_element<1, T>::type;
  using Value = typename std::tuple_element<2, T>::type;

 protected:
  void SetUp() override { Column<Key, Value>::fill(_sorted_data.entries); }

  void TearDown() override {
    // do nothing
  }

  Index _index;
  // Entries are not allowed to have a value that is 0, since the used ART implementation (from ARTSynchronized) and
  // the used judy array implementation does not support value 0.
  const Data<Key, Value> _sorted_data{
      std::vector<Key>{11, 12, 14, 16, 17, 18, 21, 22, 24, 25, 28, 30, 31, 33, 34, 36, 37, 38, 42, 43},
      std::vector<Value>{26, 29, 12, 15, 21, 3, 9, 25, 19, 2, 23, 4, 17, 6, 16, 1, 7, 20, 27, 28},
      std::vector<std::pair<Key, Value>>{{11, 26}, {12, 29}, {14, 12}, {16, 15}, {17, 21}, {18, 3},  {21, 9},
                                         {22, 25}, {24, 19}, {25, 2},  {28, 23}, {30, 4},  {31, 17}, {33, 6},
                                         {34, 16}, {36, 1},  {37, 7},  {38, 20}, {42, 27}, {43, 28}},
      std::vector<std::vector<float>>{{11}, {12}, {14}, {16}, {17}, {18}, {21}, {22}, {24}, {25},
                                      {28}, {30}, {31}, {33}, {34}, {36}, {37}, {38}, {42}, {43}}};
};

TYPED_TEST_SUITE(CompetitorUniqueKeysTest, CompetitorTestingTypes);

// Test cases are written with the assumption that all competitors support at least the IndexOperationTypes
// InsertEntry, EraseEntry and EqualityLookup

TYPED_TEST(CompetitorUniqueKeysTest, SupportBulkLoad) {
  using Index = typename std::tuple_element<0, TypeParam>::type;
  if (Index::supports(IndexOperationType::BulkLoad)) {
    EXPECT_NO_THROW(this->_index.bulk_load(this->_sorted_data));
  } else {
    EXPECT_THROW(this->_index.bulk_load(this->_sorted_data), std::logic_error);
  }
}

TYPED_TEST(CompetitorUniqueKeysTest, SupportBulkInsert) {
  using Index = typename std::tuple_element<0, TypeParam>::type;
  if (Index::supports(IndexOperationType::BulkInsert)) {
    EXPECT_NO_THROW(this->_index.bulk_insert(this->_sorted_data));
  } else {
    EXPECT_THROW(this->_index.bulk_insert(this->_sorted_data), std::logic_error);
  }
}

TYPED_TEST(CompetitorUniqueKeysTest, SupportBulkErase) {
  using Index = typename std::tuple_element<0, TypeParam>::type;
  if (Index::supports(IndexOperationType::BulkErase)) {
    EXPECT_NO_THROW(this->_index.bulk_erase(this->_sorted_data));
  } else {
    EXPECT_THROW(this->_index.bulk_erase(this->_sorted_data), std::logic_error);
  }
}

TYPED_TEST(CompetitorUniqueKeysTest, SupportInsertEntry) {
  using Index = typename std::tuple_element<0, TypeParam>::type;
  if (Index::supports(IndexOperationType::InsertEntry)) {
    EXPECT_NO_THROW(this->_index.insert(this->_sorted_data.entries[0].first, this->_sorted_data.entries[0].second));
  } else {
    EXPECT_THROW(this->_index.insert(this->_sorted_data.entries[0].first, this->_sorted_data.entries[0].second),
                 std::logic_error);
  }
}

TYPED_TEST(CompetitorUniqueKeysTest, SupportEraseEntry) {
  using Index = typename std::tuple_element<0, TypeParam>::type;
  if (Index::supports(IndexOperationType::EraseEntry)) {
    EXPECT_NO_THROW(this->_index.erase(this->_sorted_data.entries[0].first, this->_sorted_data.entries[0].second));
  } else {
    EXPECT_THROW(this->_index.erase(this->_sorted_data.entries[0].first, this->_sorted_data.entries[0].second),
                 std::logic_error);
  }
}

TYPED_TEST(CompetitorUniqueKeysTest, SupportEqualityLookup) {
  using Index = typename std::tuple_element<0, TypeParam>::type;
  if (Index::supports(IndexOperationType::EqualityLookup)) {
    EXPECT_NO_THROW(this->_index.equality_lookup(this->_sorted_data.entries[0].first));
  } else {
    EXPECT_THROW(this->_index.equality_lookup(this->_sorted_data.entries[0].first), std::logic_error);
  }
}

TYPED_TEST(CompetitorUniqueKeysTest, SupportRangeLookup) {
  using Index = typename std::tuple_element<0, TypeParam>::type;
  if (Index::supports(IndexOperationType::RangeLookup)) {
    EXPECT_NO_THROW(
        this->_index.range_lookup(this->_sorted_data.entries[0].first, this->_sorted_data.entries[0].first));
  } else {
    EXPECT_THROW(this->_index.range_lookup(this->_sorted_data.entries[0].first, this->_sorted_data.entries[0].first),
                 std::logic_error);
  }
}

// insert multiple entries using the bulk_load function
TYPED_TEST(CompetitorUniqueKeysTest, BulkLoadOnEmptyIndex) {
  using Index = typename std::tuple_element<0, TypeParam>::type;
  if (!Index::supports(IndexOperationType::BulkLoad)) {
    EXPECT_THROW(this->_index.bulk_load(this->_sorted_data), std::logic_error);
    return;
  }

  this->_index.bulk_load(this->_sorted_data);
  for (const auto& entry : this->_sorted_data.entries) {
    const auto result = this->_index.equality_lookup(entry.first);
    ASSERT_EQ(result.size(), 1u);
    ASSERT_EQ(result[0], entry.second);
  }
}

// insert multiple entries using the bulk insert function to a non-empty index. The values to insert are sorted but
// the already stored entry's key is greater than the key of the first entry to insert.
TYPED_TEST(CompetitorUniqueKeysTest, BulkInsert) {
  using Index = typename std::tuple_element<0, TypeParam>::type;
  if (!Index::supports(IndexOperationType::BulkInsert)) {
    EXPECT_THROW(this->_index.bulk_insert(this->_sorted_data), std::logic_error);
    return;
  }

  this->_index.insert(100, 200);
  {
    const auto result = this->_index.equality_lookup(100);
    ASSERT_EQ(result.size(), 1u);
    ASSERT_EQ(result[0], 200);
  }
  this->_index.bulk_insert(this->_sorted_data);
  for (const auto& entry : this->_sorted_data.entries) {
    const auto result = this->_index.equality_lookup(entry.first);
    ASSERT_EQ(result.size(), 1u);
    ASSERT_EQ(result[0], entry.second);
  }
  {
    const auto result = this->_index.equality_lookup(100);
    ASSERT_EQ(result.size(), 1u);
    ASSERT_EQ(result[0], 200);
  }
}

// insert multiple entries using the bulk load function to a non-empty index. The values to insert are sorted.
TYPED_TEST(CompetitorUniqueKeysTest, BulkLoad) {
  using Index = typename std::tuple_element<0, TypeParam>::type;
  if (!Index::supports(IndexOperationType::BulkLoad)) {
    EXPECT_THROW(this->_index.bulk_load(this->_sorted_data), std::logic_error);
    return;
  }

  this->_index.bulk_load(this->_sorted_data);
  for (const auto& entry : this->_sorted_data.entries) {
    const auto result = this->_index.equality_lookup(entry.first);
    ASSERT_EQ(result.size(), 1u);
    ASSERT_EQ(result[0], entry.second);
  }
}

// erase multiple entries using the bulk erase function
TYPED_TEST(CompetitorUniqueKeysTest, BulkErase) {
  using Index = typename std::tuple_element<0, TypeParam>::type;
  if (!Index::supports(IndexOperationType::BulkErase)) {
    EXPECT_THROW(this->_index.bulk_erase(this->_sorted_data), std::logic_error);
    return;
  }

  // store all entries
  for (const auto& entry : this->_sorted_data.entries) {
    this->_index.insert(entry.first, entry.second);
  }

  // check if all entries are stored
  for (const auto& entry : this->_sorted_data.entries) {
    const auto result = this->_index.equality_lookup(entry.first);
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], entry.second);
  }

  this->_index.bulk_erase(this->_sorted_data);
  // check if all entries are deleted
  for (const auto& entry : this->_sorted_data.entries) {
    const auto result = this->_index.equality_lookup(entry.first);
    ASSERT_EQ(result.size(), 0u);
  }
}

// insert one entry using the single insert function
TYPED_TEST(CompetitorUniqueKeysTest, SingleInsertOneEntry) {
  const auto& stored_entry = this->_sorted_data.entries[0];
  const auto& missing_entry = this->_sorted_data.entries[1];

  // no value is stored yet
  const auto result_before_insert = this->_index.equality_lookup(stored_entry.first);
  EXPECT_TRUE(result_before_insert.empty());

  // insert the entry to be stored
  this->_index.insert(stored_entry.first, stored_entry.second);

  // check if values are stored
  {
    const auto result_stored_key = this->_index.equality_lookup(stored_entry.first);
    ASSERT_EQ(result_stored_key.size(), 1u);
    EXPECT_EQ(result_stored_key[0], stored_entry.second);
    const auto result_missing_key = this->_index.equality_lookup(missing_entry.first);
    EXPECT_TRUE(result_missing_key.empty());
  }

  // insert the entry to be stored
  this->_index.insert(stored_entry.first, 200);
  const auto result_stored_key = this->_index.equality_lookup(stored_entry.first);
  using Index = typename std::tuple_element<0, TypeParam>::type;
  if (Index::supports_key_duplicates()) {
    ASSERT_EQ(result_stored_key.size(), 2u);
    EXPECT_EQ(result_stored_key[0], stored_entry.second);
    EXPECT_EQ(result_stored_key[1], 200);
  } else {
    if (Index::type() == IndexType::JudyArray) {  // JudyArray overrides an existing key
      ASSERT_EQ(result_stored_key.size(), 1u);
      EXPECT_EQ(result_stored_key[0], 200);
    } else {
      ASSERT_EQ(result_stored_key.size(), 1u);
      EXPECT_EQ(result_stored_key[0], stored_entry.second);
    }
  }
}

// insert multiple entries using the single insert function
TYPED_TEST(CompetitorUniqueKeysTest, SingleInsertMultipleEntries) {
  // initially, no entry is stored
  for (const auto& entry : this->_sorted_data.entries) {
    const auto result = this->_index.equality_lookup(entry.first);
    EXPECT_TRUE(result.empty());
  }

  // store all entries
  for (const auto& entry : this->_sorted_data.entries) {
    this->_index.insert(entry.first, entry.second);
    const auto result = this->_index.equality_lookup(entry.first);
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], entry.second);
  }

  // check if all entries are stored
  for (const auto& entry : this->_sorted_data.entries) {
    const auto result = this->_index.equality_lookup(entry.first);
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], entry.second);
  }

  const auto result_missing_key = this->_index.equality_lookup(100);
  EXPECT_TRUE(result_missing_key.empty());
}

// erase one entry using the single erase function
TYPED_TEST(CompetitorUniqueKeysTest, SingleEraseOneEntry) {
  const auto& entry = this->_sorted_data.entries[0];

  const auto result_before_insert = this->_index.equality_lookup(entry.first);
  EXPECT_TRUE(result_before_insert.empty());
  this->_index.insert(entry.first, entry.second);
  const auto result_after_insert = this->_index.equality_lookup(entry.first);
  EXPECT_EQ(result_after_insert.size(), 1u);
  {
    this->_index.erase(entry.first, entry.second);
    const auto result_after_erase = this->_index.equality_lookup(entry.first);
    EXPECT_TRUE(result_after_erase.empty());
  }
  {
    this->_index.erase(entry.first, entry.second);
    const auto result_after_erase = this->_index.equality_lookup(entry.first);
    EXPECT_TRUE(result_after_erase.empty());
  }
}

// erase multiple entries using the single erase function
TYPED_TEST(CompetitorUniqueKeysTest, SingleEraseMultipleEntries) {
  const auto& entries = this->_sorted_data.entries;

  // store all entries
  for (const auto& entry : entries) {
    this->_index.insert(entry.first, entry.second);
  }

  // check if all entries are stored
  for (const auto& entry : entries) {
    const auto result = this->_index.equality_lookup(entry.first);
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], entry.second);
  }

  auto erased_elements_counter = 0u;

  for (const auto& entry : entries) {
    this->_index.erase(entry.first, entry.second);
    ++erased_elements_counter;
    SCOPED_TRACE("Entry erased: " + std::to_string(entry.first) + ", " + std::to_string(entry.second));

    for (auto idx = 0u; idx < entries.size(); ++idx) {
      const auto result = this->_index.equality_lookup(entries[idx].first);
      SCOPED_TRACE("Value lookup for key: " + std::to_string(entries[idx].first));
      if (idx < erased_elements_counter) {
        ASSERT_EQ(result.size(), 0u);
      } else {
        ASSERT_EQ(result.size(), 1u);
        EXPECT_EQ(result[0], entries[idx].second);
      }
    }
  }
}

// not needed since equality lookup is used in almost all other tests.
// TYPED_TEST(CompetitorUniqueKeysTest, EqualityLookup) {

TYPED_TEST(CompetitorUniqueKeysTest, RangeLookupIncludingFirstEntry) {
  // store all entries
  for (const auto& entry : this->_sorted_data.entries) {
    this->_index.insert(entry.first, entry.second);
  }

  using Key = typename std::tuple_element<1, TypeParam>::type;
  const auto range = std::pair<Key, Key>{11, 25};

  using Index = typename std::tuple_element<0, TypeParam>::type;
  if (!Index::supports(IndexOperationType::RangeLookup)) {
    EXPECT_THROW(this->_index.range_lookup(range.first, range.second), std::logic_error);
    return;
  }

  const auto result = this->_index.range_lookup(range.first, range.second);
  ASSERT_EQ(result.size(), 10u);
  for (auto idx = 0u; idx < result.size(); ++idx) {
    ASSERT_EQ(result[idx], this->_sorted_data.entries[idx].second);
  }
}

TYPED_TEST(CompetitorUniqueKeysTest, RangeLookupIncludingLastEntry) {
  // store all entries
  for (const auto& entry : this->_sorted_data.entries) {
    this->_index.insert(entry.first, entry.second);
  }

  using Key = typename std::tuple_element<1, TypeParam>::type;
  const auto range = std::pair<Key, Key>{28, 43};

  using Index = typename std::tuple_element<0, TypeParam>::type;
  if (!Index::supports(IndexOperationType::RangeLookup)) {
    EXPECT_THROW(this->_index.range_lookup(range.first, range.second), std::logic_error);
    return;
  }
  const auto result = this->_index.range_lookup(range.first, range.second);
  ASSERT_EQ(result.size(), 10u);
  const auto entry_start_index = 10;
  for (auto idx = 0u; idx < result.size(); ++idx) {
    ASSERT_EQ(result[idx], this->_sorted_data.entries[entry_start_index + idx].second);
  }
}

TYPED_TEST(CompetitorUniqueKeysTest, RangeLookupWithoutFirstAndLastEntries) {
  // store all entries
  for (const auto& entry : this->_sorted_data.entries) {
    this->_index.insert(entry.first, entry.second);
  }

  using Key = typename std::tuple_element<1, TypeParam>::type;
  const auto range = std::pair<Key, Key>{28, 34};

  using Index = typename std::tuple_element<0, TypeParam>::type;
  if (!Index::supports(IndexOperationType::RangeLookup)) {
    EXPECT_THROW(this->_index.range_lookup(range.first, range.second), std::logic_error);
    return;
  }

  const auto result = this->_index.range_lookup(range.first, range.second);
  ASSERT_EQ(result.size(), 5u);
  const auto entry_start_index = 10;
  for (auto idx = 0u; idx < result.size(); ++idx) {
    ASSERT_EQ(result[idx], this->_sorted_data.entries[entry_start_index + idx].second);
  }
}

TYPED_TEST(CompetitorUniqueKeysTest, RangeLookupIsEqualityLookup) {
  // store all entries
  for (const auto& entry : this->_sorted_data.entries) {
    this->_index.insert(entry.first, entry.second);
  }

  const auto entry_to_lookup = this->_sorted_data.entries[9];

  using Index = typename std::tuple_element<0, TypeParam>::type;
  if (!Index::supports(IndexOperationType::RangeLookup)) {
    EXPECT_THROW(this->_index.range_lookup(entry_to_lookup.first, entry_to_lookup.first), std::logic_error);
    return;
  }

  const auto result = this->_index.range_lookup(entry_to_lookup.first, entry_to_lookup.first);
  ASSERT_EQ(result.size(), 1u);
  ASSERT_EQ(result[0], entry_to_lookup.second);
}

}  // namespace imib
