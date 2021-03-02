#pragma once

#include <string>
#include <vector>

#include "tlx/container/btree_map.hpp"

// References:
// https://panthema.net/2018/0528-tlx-library/
// https://github.com/tlx/tlx/tree/master/tlx/container

// The BTreeTLX explicitly has a bulk_load function that required a sorted range of entries.
// To use this function, the index has to be empty before it is called.

namespace imib {

template <typename Key, typename Value>
class BTreeTlx {
 public:
  static bool supports_key_duplicates() { return false; }

  static bool supports(const IndexOperationType operation_type) {
    switch (operation_type) {
      case IndexOperationType::BulkLoad:
        return true;
      case IndexOperationType::BulkInsert:
        return true;
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

  static const std::string name() { return "TLX B+ Tree"; }

  static IndexType type() { return IndexType::BTreeTLX; }

  // sorted entries are required!
  void bulk_load(const Data<Key, Value>& data) { _btree_map.bulk_load(data.entries.cbegin(), data.entries.cend()); }

  void bulk_insert(const Data<Key, Value>& data) { _btree_map.insert(data.entries.cbegin(), data.entries.cend()); }

  void bulk_erase(const Data<Key, Value>& data) { throw std::logic_error("Not supported."); }

  void insert(const Key& key, const Value& value) { _btree_map.insert2(key, value); }

  void erase(const Key& key, const Value& value) { _btree_map.erase(key); }

  std::vector<Value> equality_lookup(const Key& key) const {
    const auto found = _btree_map.find(key);
    if (found != _btree_map.end()) {
      return std::vector<Value>{found->second};
    }
    return std::vector<Value>{};
    // for multi map
    // const auto range = _btree_multimap.equal_range(key);
    // auto result = std::vector<Value>(std::distance(range.first, range.second));
    // auto idx = 0u;
    // for (auto iter = range.first; iter != range.second; ++iter) {
    //   result[idx] = iter->second;
    //   ++idx;
    // }
    // return result;
  }

  std::vector<Value> range_lookup(const Key& lower_bound_key, const Key& upper_bound_key) const {
    auto lower_bound = _btree_map.lower_bound(lower_bound_key);
    const auto upper_bound = _btree_map.upper_bound(upper_bound_key);

    auto result = std::vector<Value>{};
    result.reserve(std::distance(lower_bound, upper_bound));
    for (; lower_bound != upper_bound; ++lower_bound) {
      result.emplace_back(lower_bound->second);
    }
    return result;
  }

 protected:
  tlx::btree_map<Key, Value> _btree_map;
};

}  // namespace imib
