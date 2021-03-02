#pragma once

#include <memory>
#include <string>

#include "base_benchmark_case.hpp"
#include "types.hpp"

namespace imib {

template <typename Index, typename Key, typename Value>
class AbstractBenchmarkCase : public BaseBenchmarkCase {
 public:
  explicit AbstractBenchmarkCase(const Data<Key, Value>& data) : _data(data) {
    // do nothing
  }

  ~AbstractBenchmarkCase() override = default;

  // returns the name of the used index
  const std::string index_name() const override { return Index::name(); }

  // returns the key type as string
  const std::string key_type() const override { return Type<Key>::name(); }

  // returns the value type as string
  const std::string value_type() const override { return Type<Value>::name(); }

  // return the size of the data
  size_t data_size() const override { return _data.entries.size(); }

 protected:
  const Data<Key, Value>& _data;
};

}  // namespace imib
