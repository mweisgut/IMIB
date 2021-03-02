#pragma once

#include <cassert>
#include <string>
#include <type_traits>
#include <vector>

#include "BBTree.h"

// supports
// - only unique float keys, however, we cast the key
// - only uint32_t values!

// References:
// https://openproceedings.org/2019/conf/edbt/EDBT19_paper_27.pdf
// https://www2.informatik.hu-berlin.de/~sprengsz/bb-tree/

namespace imib {

template <typename Key, typename Value>
class BbTree {
 public:
  static bool supports_key_duplicates() { return false; }
  // SearchObject returns -1 as uint32_t if no matching data object has been found.
  static constexpr auto NO_SEARCH_RESULT = static_cast<uint32_t>(-1);

  static bool supports(const IndexOperationType operation_type) {
    switch (operation_type) {
      case IndexOperationType::BulkLoad:
        return true;
      case IndexOperationType::BulkInsert:
        return false;
      case IndexOperationType::BulkErase:
        return false;
      case IndexOperationType::InsertEntry:
        return true;
      case IndexOperationType::EraseEntry:
        return true;
      case IndexOperationType::EqualityLookup:
        return true;
      case IndexOperationType::RangeLookup:
        return true;
    }
  }

  // BBTree(size_t dimensions, size_t num_threads)
  BbTree() : _bb_tree(BBTree(1, 1)) {
    static_assert(std::is_arithmetic<Key>::value, "Not an arithmetic type");
    static_assert(std::is_same<Value, uint32_t>::value, "Only uint32_t values are supported.");
    // do nothing
  }

  static const std::string name() { return "BB-Tree"; }

  static IndexType type() { return IndexType::BBTree; }

  // inserts on already built index structures are not allowed
  void bulk_load(const Data<Key, Value>& data) { _bb_tree.BulkInsert(data.keys_float, data.values); }

  void bulk_insert(const Data<Key, Value>& data) { throw std::logic_error("Not supported."); }

  void bulk_erase(const Data<Key, Value>& data) { throw std::logic_error("Not supported."); }

  void insert(const Key& key, const uint32_t value) {
    // narrowing issue! Key could exceed float max values
    assert(key == static_cast<Key>(static_cast<float>(key)));
    _bb_tree.InsertObject(std::vector<float>{static_cast<float>(key)}, value);
  }

  void erase(const Key& key, const uint32_t value) {
    // narrowing issue! Key could exceed float max values
    assert(key == static_cast<Key>(static_cast<float>(key)));
    _bb_tree.DeleteObject(std::vector<float>{static_cast<float>(key)});
  }

  std::vector<uint32_t> equality_lookup(const Key& key) const {
    // narrowing issue! Key could exceed float max values
    assert(key == static_cast<Key>(static_cast<float>(key)));
    const auto result = _bb_tree.SearchObject(std::vector<float>{static_cast<float>(key)});
    if (result == NO_SEARCH_RESULT) {
      return std::vector<uint32_t>{};
    } else {
      return std::vector<uint32_t>{result};
    }
  }

  std::vector<uint32_t> range_lookup(const Key& lower_bound_key, const Key& upper_bound_key) {
    // narrowing issue! Key could exceed float max values
    assert(lower_bound_key == static_cast<Key>(static_cast<float>(lower_bound_key)));
    assert(upper_bound_key == static_cast<Key>(static_cast<float>(upper_bound_key)));
    return _bb_tree.SearchRange(std::vector<float>{static_cast<float>(lower_bound_key)},
                                std::vector<float>{static_cast<float>(upper_bound_key)});
  }

 protected:
  BBTree _bb_tree;
};

}  // namespace imib
