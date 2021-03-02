#pragma once

#include <cassert>
#include <string>
#include <vector>

#include "robin_hood.h"  // NOLINT

// supports: only unique keys, no support for key duplicates

namespace imib {

template <typename Key, typename Value>
class HashMapRobinHoodFlat {
 public:
  static bool supports_key_duplicates() { return false; }

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

  static const std::string name() { return "RH Flat Map"; }

  static IndexType type() { return IndexType::RobinHoodFlatMap; }

  void bulk_load(const Data<Key, Value>& data) { throw std::logic_error("Not supported."); }

  void bulk_insert(const Data<Key, Value>& data) { _map.insert(data.entries.cbegin(), data.entries.cend()); }

  void bulk_erase(const Data<Key, Value>& data) { throw std::logic_error("Not supported."); }

  void insert(const Key& key, const Value& value) { _map.insert({key, value}); }

  void erase(const Key& key, const Value& value) { _map.erase(key); }

  std::vector<Value> equality_lookup(const Key& key) const {
    const auto result = _map.find(key);
    if (result != _map.cend()) {
      return std::vector<Value>{result->second};
    } else {
      return std::vector<Value>{};
    }
  }

  std::vector<Value> range_lookup(const Key& lower_bound_key, const Key& upper_bound_key) const {
    throw std::logic_error("Not supported.");
  }

 protected:
  robin_hood::unordered_flat_map<Key, Value> _map;
};

}  // namespace imib
