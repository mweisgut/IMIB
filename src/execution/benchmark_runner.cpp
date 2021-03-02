#include "benchmark_runner.hpp"

#include <cassert>
#include <iostream>

#include "evaluation/report.hpp"
#include "evaluation/report_exporter.hpp"
#include "evaluation/timer.hpp"
#include "types.hpp"

namespace imib {

BenchmarkRunner::BenchmarkRunner(const std::vector<std::shared_ptr<BaseBenchmarkCase>>& cases,
                                 const BenchmarkConfiguration config)
    : _cases(cases), _config(std::move(config)), _report(Report()) {
  assert(_config.threads == 1);  // Multi-threaded execution not supported yet.
}

void BenchmarkRunner::run() {
  auto print_info = [](const auto bench_case) {
    std::cout << "Run " << bench_case->name() << ", " << bench_case->index_name()
              << ", key type: " << bench_case->key_type() << ", value type: " << bench_case->value_type() << std::endl;
  };

  std::vector<BenchmarkCaseStatistics> case_statistics(_cases.size());
  auto case_index = size_t{0};
  for (const auto& bench_case : _cases) {
    print_info(bench_case);
    std::vector<ExecutionStatistics> execution_statistics(_config.iterations);
    for (auto execution_index = 0u; execution_index < _config.iterations; ++execution_index) {
      std::cout << "iteration " << execution_index + 1 << std::endl;
      Timer timer{};
      _clear_cache();
      const auto current_execution_stats = bench_case->execute();
      execution_statistics[execution_index] = std::move(current_execution_stats);
      // TODO(Marcel) execute in parallel
    }
    case_statistics[case_index] =
        BenchmarkCaseStatistics{bench_case->name(),       bench_case->index_name(), bench_case->key_type(),
                                bench_case->value_type(), bench_case->data_size(),  std::move(execution_statistics)};
    ++case_index;
  }
  _report.benchmark_configuration = _config;
  _report.case_statistics = std::move(case_statistics);

  if (!_config.output_file_name.empty()) {
    export_result(_config.output_file_name);
  } else {
    print_result();
  }
}

void BenchmarkRunner::export_result(const std::string& output_file_path) const {
  ReportExporter exporter(_report, output_file_path);
  exporter.execute();
}

void BenchmarkRunner::print_result() const { std::cout << _report << std::endl; }

void BenchmarkRunner::_clear_cache() const {
  std::vector<uint8_t> data = std::vector<uint8_t>();
  const auto larger_than_cache = 200 * 1024 * 1024;  // 200 MiB
  data.resize(larger_than_cache, 42);
  for (size_t idx = 0; idx < data.size(); ++idx) {
    data[idx] += 1;
  }
  data.resize(0);
}

}  // namespace imib
