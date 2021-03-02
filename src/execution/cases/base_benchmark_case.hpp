#pragma once

#include "evaluation/report.hpp"

namespace imib {

class BaseBenchmarkCase {
 public:
  BaseBenchmarkCase() = default;
  BaseBenchmarkCase(const BaseBenchmarkCase&) = delete;
  virtual ~BaseBenchmarkCase() = default;
  const BaseBenchmarkCase& operator=(const BaseBenchmarkCase&) = delete;

  // executes the benchmark scenario and returns the duration in nanoseconds
  virtual const ExecutionStatistics execute() const = 0;

  // returns the name of the benchmark case
  virtual const std::string name() const = 0;

  // returns the name of the used index
  virtual const std::string index_name() const = 0;

  // returns the key type as string
  virtual const std::string key_type() const = 0;

  // returns the value type as string
  virtual const std::string value_type() const = 0;

  // return the size of the data
  virtual size_t data_size() const = 0;
};

}  // namespace imib
