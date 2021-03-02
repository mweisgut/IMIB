#pragma once

#include <string>
#include <unordered_map>
#include <vector>

// supports non-unique keys

namespace imib {

template <typename Key, typename Value>
class UnorderedMap {
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

  static const std::string name() { return "STD Hash Map"; }

  static IndexType type() { return IndexType::UnorderedMap; }

  void bulk_load(const Data<Key, Value>& data) { throw std::logic_error("Not supported."); }

  void bulk_insert(const Data<Key, Value>& data) { _map.insert(data.entries.cbegin(), data.entries.cend()); }

  void bulk_erase(const Data<Key, Value>& data) { throw std::logic_error("Not supported."); }

  void insert(const Key& key, const Value& value) { _map.insert({key, value}); }

  void erase(const Key& key, const Value& value) {
    _map.erase(key);
    // unordered multimap
    // const auto range = _map.equal_range(key);
    // auto iter = range.first;
    // for (; iter != range.second; ++iter) {
    //   if (iter->second == value) {
    //     _map.erase(iter);
    //     break;
    //   }
    // }
  }

  std::vector<Value> equality_lookup(const Key& key) const {
    const auto found = _map.find(key);
    if (found != _map.cend()) {
      return std::vector<Value>{found->second};
    }
    return std::vector<Value>{};
    // unorderex multimap
    // const auto range = _map.equal_range(key);
    // auto result = std::vector<Value>{};
    // result.reserve(std::distance(range.first, range.second));
    // auto iter = range.first;
    // for (; iter != range.second; ++iter) {
    //   result.emplace_back(iter->second);
    // }
    // return result;
  }

  std::vector<Value> range_lookup(const Key& lower_bound_key, const Key& upper_bound_key) const {
    throw std::logic_error("Not supported.");
  }

 protected:
  std::unordered_map<Key, Value> _map;
};

}  // namespace imib
