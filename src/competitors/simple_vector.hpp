#pragma once

#include <string>
#include <vector>

#include "types.hpp"

namespace imib {

// TODO(Marcel) separated simple and sorted vector would be interesting as well: Storing the keys and values in
// separate vectors. Searching for a key or a range of key would be more cache efficient in the key vector since more
// entries would fit into the machine caches.

template <typename Key, typename Value>
class SimpleVector {
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
        return false;
    }
  }

  static const std::string name() { return "Simple Vector"; }

  static IndexType type() { return IndexType::SimpleVector; }

  void bulk_load(const Data<Key, Value>& data) { throw std::logic_error("Not supported."); }

  void bulk_insert(const Data<Key, Value>& data) {
    _entries.insert(_entries.end(), data.entries.cbegin(), data.entries.cend());
  }

  void bulk_erase(const Data<Key, Value>& data) { throw std::logic_error("Not supported."); }

  void insert(const Key& key, const Value& value) { _entries.emplace_back(key, value); }

  void erase(const Key& key, const Value& value) {
    for (auto iter = _entries.begin(); iter != _entries.end();) {
      // prerequisite: values are unique. That should be the case if we use value-position indexes
      if (iter->second == value) {
        iter = _entries.erase(iter);
      } else {
        ++iter;
      }
    }
    _entries.shrink_to_fit();
  }

  std::vector<Value> equality_lookup(const Key& key) const {
    auto result = std::vector<Value>{};
    for (auto iter = _entries.begin(); iter != _entries.end(); ++iter) {
      if (iter->first == key) {
        result.emplace_back(iter->second);
      }
    }
    return result;
  }

  std::vector<Value> range_lookup(const Key& lower_bound_key, const Key& upper_bound_key) const {
    throw std::logic_error("Not supported.");
  }

 protected:
  std::vector<std::pair<Key, Value>> _entries;
};

}  // namespace imib
