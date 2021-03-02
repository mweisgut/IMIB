#pragma once

#include <iostream>

#include <algorithm>
#include <string>
#include <vector>

#include "types.hpp"

namespace imib {

template <typename Key, typename Value>
class SortedVector {
 public:
  static bool supports_key_duplicates() { return true; }

  static bool supports(const IndexOperationType operation_type) {
    switch (operation_type) {
      case IndexOperationType::BulkLoad:
        return false;
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

  static const std::string name() { return "Sorted Vector"; }

  static IndexType type() { return IndexType::SortedVector; }

  void bulk_load(const Data<Key, Value>& data) { throw std::logic_error("Not supported."); }

  void bulk_insert(const Data<Key, Value>& data) {
    _entries.insert(_entries.end(), data.entries.cbegin(), data.entries.cend());
    std::sort(_entries.begin(), _entries.end());
  }

  void bulk_erase(const Data<Key, Value>& data) { throw std::logic_error("Not supported."); }

  // https://stackoverflow.com/questions/15048466/inserting-element-to-a-sorted-vector-and-keeping-elements-sorted/15048651
  void insert(const Key& key, const Value& value) {
    const auto compare = [](const auto search_key, const auto entry) { return search_key < entry.first; };
    _entries.insert(std::upper_bound(_entries.begin(), _entries.end(), key, compare), {key, value});
  }

  void erase(const Key& key, const Value& value) {
    using TypedEntry = std::pair<Key, Value>;
    auto iter = std::lower_bound(_entries.begin(), _entries.end(), TypedEntry{key, value});
    // reminder: we assume that the stored values are unique since they are tuple positions in a DBMS
    if (iter != _entries.cend() && iter->second == value) {
      _entries.erase(iter);
    }
  }

  std::vector<Value> equality_lookup(const Key& key) const { return range_lookup(key, key); }

  std::vector<Value> range_lookup(const Key& lower_bound_key, const Key& upper_bound_key) const {
    using TypedEntry = std::pair<Key, Value>;
    const auto compare_lower_bound = [](const TypedEntry entry, const Key& search_key) {
      return entry.first < search_key;
    };
    const auto compare_upper_bound = [](const Key& search_key, const TypedEntry entry) {
      return search_key < entry.first;
    };

    auto lower_bound = std::lower_bound(_entries.begin(), _entries.end(), lower_bound_key, compare_lower_bound);
    const auto upper_bound = std::upper_bound(_entries.begin(), _entries.end(), upper_bound_key, compare_upper_bound);

    auto result = std::vector<Value>{};
    result.reserve(std::distance(lower_bound, upper_bound));
    for (; lower_bound != upper_bound; ++lower_bound) {
      result.emplace_back(lower_bound->second);
    }
    return result;
  }

 protected:
  std::vector<std::pair<Key, Value>> _entries;
};

}  // namespace imib
