#pragma once

#include <string>
#include <tuple>
#include <vector>

namespace imib {

template <typename Key, typename Value>
struct Data {
  std::vector<Key> keys;
  std::vector<Value> values;
  std::vector<std::pair<Key, Value>> entries;
  // BBTree is a multidimensional index structure. We used with one dimension. Thus, it requires a one dimensional
  // float vectors as keys
  std::vector<std::vector<float>> keys_float;

  Data() = default;
  explicit Data(const std::vector<Key> p_keys, const std::vector<Value> p_values,
                const std::vector<std::pair<Key, Value>> p_entries, const std::vector<std::vector<float>> p_keys_float)
      : keys(p_keys), values(p_values), entries(p_entries), keys_float(p_keys_float) {}
};

template <typename Key, typename Value>
using Entries = std::vector<std::pair<Key, Value>>;

template <typename Key>
struct EqualityLookup {
  Key key;

  EqualityLookup() = default;
  explicit EqualityLookup(const Key& p_key) : key(p_key) {}
};

template <typename Key>
struct RangeLookup {
  Key lower_bound_key;
  Key upper_bound_key;

  RangeLookup() = default;
  RangeLookup(const Key& p_lower_bound_key, const Key& p_upper_bound_key)
      : lower_bound_key(p_lower_bound_key), upper_bound_key(p_upper_bound_key) {}
};

enum class KeyType { Int32, Int64, Float, Double, String };

enum class ValueType {
  UInt64
  // RowID
};

struct BenchmarkConfiguration {
  size_t iterations;
  size_t threads;
  std::string data_file;
  std::string equality_lookup_file;
  std::string range_lookup_file;
  std::string output_file_name;
};

enum class IndexType {
  ART,
  BTreeAbseil,
  BTreeTLX,
  BBTree,
  RobinHoodFlatMap,
  RobinHoodNodeMap,
  RobinMapTSL,
  SparseMapTSL,
  JudyArray,
  SimpleVector,
  SkipList,
  SortedVector,
  UnorderedMap
};

enum class IndexOperationType {
  BulkLoad,    // inserts a range of entries into an empty index
  BulkInsert,  // inserts a range of entries into a non-empty index
  BulkErase,   // erase a range of entries
  InsertEntry,
  EraseEntry,
  EqualityLookup,
  RangeLookup
};

template <typename T>
struct Type {
  static const std::string name();
};

}  // namespace imib
