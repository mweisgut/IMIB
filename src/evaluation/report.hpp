#pragma once

#include <chrono>
#include <ostream>
#include <string>
#include <vector>

#include "types.hpp"

namespace imib {

struct Measurements {
  std::chrono::nanoseconds duration;
  uint64_t index_size_bytes;

  Measurements() = default;
  Measurements(const std::chrono::nanoseconds& p_duration, const uint64_t p_index_size_bytes)
      : duration(p_duration), index_size_bytes(p_index_size_bytes) {}
};

struct ExecutionStatistics {
  std::vector<Measurements> measurements;
};

struct BenchmarkCaseStatistics {
  std::string case_name;
  std::string index_name;
  std::string key_type;
  std::string value_type;
  size_t data_size;
  std::vector<ExecutionStatistics> executions;
};

struct Report {
  BenchmarkConfiguration benchmark_configuration;
  std::vector<BenchmarkCaseStatistics> case_statistics;
};

std::ostream& operator<<(std::ostream& stream, const Report& report);

}  // namespace imib
