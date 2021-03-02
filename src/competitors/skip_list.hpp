#pragma once

#include <string>
#include <vector>

#include "skip_list.h"  // NOLINT

namespace imib {

template <typename Key, typename Value>
struct CompareKeyValuePairOnKey {
  bool operator()(const std::pair<Key, Value>& lhs, const std::pair<Key, Value>& rhs) const {
    return lhs.first < rhs.first;
  }
};

template <typename Key, typename Value>
struct SkipListEntry {
  Key key;
  Value value;

  SkipListEntry() = default;
  explicit SkipListEntry(const Key& p_key) : key(p_key), value(Value{}) {}
  SkipListEntry(const Key& p_key, const Value& p_value) : key(p_key), value(p_value) {}
  bool operator<(const SkipListEntry& rhs) const { return key < rhs.key; }
};

template <typename Key, typename Value>
class SkipList {
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
        return true;
    }
  }

  static const std::string name() { return "PG Skip List"; }

  static IndexType type() { return IndexType::SkipList; }

  void bulk_load(const Data<Key, Value>& data) { throw std::logic_error("Not supported."); }

  void bulk_insert(const Data<Key, Value>& data) { _skip_list.insert(data.entries.cbegin(), data.entries.cend()); }

  void bulk_erase(const Data<Key, Value>& data) { throw std::logic_error("Not supported."); }

  void insert(const Key& key, const Value& value) { _skip_list.insert({key, value}); }

  void erase(const Key& key, const Value& value) {
    _skip_list.erase({key, value});
    // multi skip list, non-unique values
    // // 'erase' removes all values that are evaluated as equal (using the operator<).
    // // Since data structures that allow to
    // // store key duplicates are preferred in the DMBS context. Thus, we have to fall back to use 'equal_range' and
    // // find the entries with the specified value we want to erase.
    // const auto search_entry = SkipListEntry<Key, Value>(key);
    // auto range = _skip_list.equal_range(search_entry);
    // for (; range.first != range.second; ++range.first) {
    //   if ((*range.first).value == value) {
    //     _skip_list.erase(range.first);
    //   }
    // }
  }

  std::vector<Value> equality_lookup(const Key& key) const {
    auto found = _skip_list.find({key, 0});
    if (found != _skip_list.cend()) {
      return std::vector<Value>{(*found).second};
    }
    return std::vector<Value>{};
    // multi skip list, non-unique values
    // const auto search_entry = SkipListEntry<Key, Value>(key);
    // const auto range = _skip_list.equal_range(search_entry);
    // auto result = std::vector<Value>(std::distance(range.first, range.second));
    // auto idx = 0u;
    // for (auto iter = range.first; iter != range.second; ++iter) {
    //   result[idx] = (*iter).value;
    //   ++idx;
    // }
    // return result;
  }

  std::vector<Value> range_lookup(const Key& lower_bound_key, const Key& upper_bound_key) const {
    auto lower_bound = _skip_list.lower_bound({lower_bound_key, 0});
    const auto upper_bound = _skip_list.upper_bound({upper_bound_key, 0});

    auto result = std::vector<Value>{};
    result.reserve(std::distance(lower_bound, upper_bound));
    for (; lower_bound != upper_bound; ++lower_bound) {
      result.emplace_back((*lower_bound).second);
    }
    return result;
  }

 protected:
  // we use multi skip list since is provides upper and lower bound functions
  goodliffe::multi_skip_list<std::pair<Key, Value>, CompareKeyValuePairOnKey<Key, Value>> _skip_list;
};

}  // namespace imib
