#pragma once

#include <functional>
#include <string>
#include <vector>

#include "ART/Tree.h"
#include "Key.h"

#include "data/column.hpp"
#include "types.hpp"

namespace imib {

// ART constructor required an argument of type LoadKeyFunction = void (*)(TID tid, Key &key);
// the key class supports variable length keys
// https://github.com/flode/ARTSynchronized/issues/1
// tid 0 is not supported

template <typename Key, typename Value>
void load_key(TID tid, ART::Key& art_key) {
  const Key& key = Column<Key, Value>::value_at(tid);
  char char_key[sizeof(key)];
  *reinterpret_cast<Key*>(char_key) = key;
  // alternative:
  // std::memcpy(char_key, &key, sizeof(key));
  art_key.set(char_key, sizeof(char_key));
}

template <typename Key, typename Value>
class Art {
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

  Art() : _tree(load_key<Key, Value>) {
    // do nothing
  }

  static const std::string name() { return "Unsync ART"; }

  static IndexType type() { return IndexType::ART; }

  void bulk_load(const Data<Key, Value>& data) { throw std::logic_error("Not supported."); }

  void bulk_insert(const Data<Key, Value>& data) { throw std::logic_error("Not supported."); }

  void bulk_erase(const Data<Key, Value>& data) { throw std::logic_error("Not supported."); }

  void insert(const Key& key, const Value& value) {
    const auto& tid = value;
    ART::Key art_key;
    load_key<Key, Value>(tid, art_key);
    _tree.insert(art_key, tid);
  }

  void erase(const Key& key, const Value& value) {
    ART::Key art_key;
    char char_key[sizeof(key)];
    *reinterpret_cast<Key*>(char_key) = key;
    // alternative:
    // std::memcpy(char_key, &key, sizeof(key));
    art_key.set(char_key, sizeof(char_key));
    _tree.remove(art_key, value);
  }

  std::vector<Value> equality_lookup(const Key& key) const {
    ART::Key art_key;
    char char_key[sizeof(key)];
    *reinterpret_cast<Key*>(char_key) = key;
    // alternative:
    // std::memcpy(char_key, &key, sizeof(key));
    art_key.set(char_key, sizeof(char_key));
    const auto tid = _tree.lookup(art_key);
    if (tid > 0) {
      return std::vector<Value>{tid};
    }
    return std::vector<Value>{};
  }

  std::vector<Value> range_lookup(const Key& lower_bound_key, const Key& upper_bound_key) const {
    // TODO(Marcel) implement, edit: range lookup is commented out in ARTSynchronized
    throw std::logic_error("Not supported.");
  }

 protected:
  ART_unsynchronized::Tree _tree;
};

}  // namespace imib
