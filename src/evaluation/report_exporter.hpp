#pragma once

#include <string>

namespace imib {

struct Report;

enum class OutputType { Json };

class ReportExporter {
 public:
  ReportExporter(const Report& report, const std::string& output_file_path,
                 const OutputType output_type = OutputType::Json);
  void execute() const;

 protected:
  const Report& _report;
  const std::string _output_file_path;
  const OutputType _output_type;
};

}  // namespace imib
