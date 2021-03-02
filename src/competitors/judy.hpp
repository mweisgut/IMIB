#pragma once

#include <string>
#include <vector>

#include "judyLArray.h"

// Both template parameters must be the same size as a void*
// does not support value 0
// judyLArray supports only unique values.
// judyL2Array supports multiple values. Values for the same key are stored in a vector. If an entry shall be removed,
// all values for a specific key are removed.

namespace imib {

template <typename Key, typename Value>
class Judy {
 public:
  static bool supports_key_duplicates() { return false; }

  static bool supports(const IndexOperationType operation_type) {
    switch (operation_type) {
      case IndexOperationType::BulkLoad:
        return false;
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
        return false;
    }
  }

  static const std::string name() { return "MP Judy"; }

  static IndexType type() { return IndexType::JudyArray; }

  void bulk_load(const Data<Key, Value>& data) { throw std::logic_error("Not supported."); }

  void bulk_insert(const Data<Key, Value>& data) { throw std::logic_error("Not supported."); }

  void bulk_erase(const Data<Key, Value>& data) { throw std::logic_error("Not supported."); }

  // if a pair with the given key already exists, it is overwritten.
  void insert(const Key& key, const Value& value) { _judy.insert(key, value); }

  void erase(const Key& key, const Value& value) { _judy.removeEntry(key); }

  std::vector<Value> equality_lookup(const Key& key) {
    const auto result = _judy.find(key);
    if (_judy.success()) {
      return std::vector<Value>{result};
    } else {
      return std::vector<Value>{};
    }
  }

  std::vector<Value> range_lookup(const Key& lower_bound_key, const Key& upper_bound_key) const {
    // atOrAfter(key) function is provided, but atOrBefore is not.
    throw std::logic_error("Not supported.");
  }

 protected:
  judyLArray<Key, Value> _judy;
};

}  // namespace imib
