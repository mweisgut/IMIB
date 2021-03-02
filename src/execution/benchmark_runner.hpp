#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

#include "cases/base_benchmark_case.hpp"
#include "evaluation/report.hpp"

namespace imib {

class BenchmarkCase;

class BenchmarkRunner {
 public:
  BenchmarkRunner(const std::vector<std::shared_ptr<BaseBenchmarkCase>>& benchmark_cases,
                  const BenchmarkConfiguration config);
  void run();
  void export_result(const std::string& output_file_path) const;
  void print_result() const;

 protected:
  void _clear_cache() const;

  const std::vector<std::shared_ptr<BaseBenchmarkCase>>& _cases;
  const BenchmarkConfiguration _config;

  Report _report;
};

}  // namespace imib
