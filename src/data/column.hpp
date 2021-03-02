#pragma once

#include <cassert>
#include <memory>

#include "types.hpp"

namespace imib {

// The ART (see ARTSynchronized) needs access to the entries in order to load the key (= attribute value) for a given
// tuple id (tid).
// This is a quick and dirty solution! Use Column with caution.
template <typename AttributeValue, typename TID>
class Column {
 public:
  static std::shared_ptr<std::vector<AttributeValue>> values;
  // the caller has to check that Column::values is not nullptr. Without bounds checking.
  // - 1 since the TID starts at 1.
  static AttributeValue& value_at(const size_t tid) { return (*values)[tid - 1]; }

  static void fill(const std::vector<std::pair<AttributeValue, TID>> entries) {
    // find the maximum tid of the entries
    size_t max_tid = 0;
    for (const auto& entry : entries) {
      const auto tid = entry.second;
      // tid is not allowed to be 0 since the used ART implementation (from ARTSynchronized) and the used judy array do
      // not support 0 as tid
      assert(tid > 0);
      if (tid > max_tid) {
        max_tid = tid;
      }
    }

    // setup the column

    // create column with max_tid values
    values = std::make_shared<std::vector<AttributeValue>>(max_tid);
    // set the cells that were indexed
    for (const auto& entry : entries) {
      // - 1 since the TID starts at 1.
      (*values)[entry.second - 1] = entry.first;
    }
  }

 protected:
  Column() {
    // do nothing
  }
};

template <typename AttributeValue, typename TID>
std::shared_ptr<std::vector<AttributeValue>> Column<AttributeValue, TID>::values = nullptr;

}  // namespace imib
