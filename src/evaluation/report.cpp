#include "report.hpp"

namespace imib {

std::ostream& operator<<(std::ostream& stream, const Report& report) {
  stream << "==== Configuration ====" << std::endl
         << "iterations: " << report.benchmark_configuration.iterations << std::endl
         << "threads:    " << report.benchmark_configuration.threads << std::endl
         << "======== Cases ========" << std::endl;
  for (const auto& case_stats : report.case_statistics) {
    stream << "case name:  " << case_stats.case_name << std::endl
           << "index name: " << case_stats.index_name << std::endl
           << "key type:   " << case_stats.key_type << std::endl
           << "value type: " << case_stats.value_type << std::endl
           << "data size:" << case_stats.data_size << std::endl
           << "executions: " << std::endl;
    for (const auto& execution : case_stats.executions) {
      stream << "  measurements:" << std::endl;
      for (const auto& measurement : execution.measurements) {
        stream << "    duration: " << measurement.duration.count() << " ns, "
               << "index size: " << measurement.index_size_bytes << " bytes" << std::endl;
      }
    }
    stream << "-----------------------" << std::endl;
  }
  return stream;
}

}  // namespace imib
