#include "report_exporter.hpp"

#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "nlohmann/json.hpp"

#include "report.hpp"

namespace imib {

ReportExporter::ReportExporter(const Report& report, const std::string& output_file_path, const OutputType output_type)
    : _report(report), _output_file_path(output_file_path), _output_type(output_type) {
  assert(_output_type == OutputType::Json);  // Only Json files can be exported for now.
}

void ReportExporter::execute() const {
  std::ofstream output_file(_output_file_path);

  nlohmann::json json_report;
  json_report["configuration"] = nlohmann::json{{"iterations", _report.benchmark_configuration.iterations},
                                                {"threads", _report.benchmark_configuration.threads}};

  auto json_cases = nlohmann::json::array();
  for (const auto& case_stats : _report.case_statistics) {
    auto json_executions = nlohmann::json::array();
    for (const auto& execution : case_stats.executions) {
      auto json_measurements = nlohmann::json::array();  // multiple measurements could be saved in one execution
      for (const auto& measurement : execution.measurements) {
        json_measurements.push_back(
            {{"duration_ns", measurement.duration.count()}, {"index_size_bytes", measurement.index_size_bytes}});
      }
      json_executions.push_back({"measurements", json_measurements});
    }
    json_cases.push_back(nlohmann::json{{"case_name", case_stats.case_name},
                                        {"index_name", case_stats.index_name},
                                        {"key_type", case_stats.key_type},
                                        {"value_type", case_stats.value_type},
                                        {"data_size", case_stats.data_size},
                                        {"executions", json_executions}});
  }
  json_report["cases"] = json_cases;

  output_file << std::setw(2) << json_report << std::endl;
}

}  // namespace imib
