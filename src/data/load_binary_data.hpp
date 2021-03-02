#pragma once

#include <fstream>
#include <ios>

namespace imib {

template <typename T>
static std::vector<T> load_binary_data(const std::string& filename) {
  std::vector<T> values;
  std::ifstream input_file(filename, std::ios::binary);
  if (!input_file.is_open()) {
    throw std::logic_error("Opening " + filename + " failed.");
  }
  // read the value count which is stored as first value
  uint64_t value_count;
  input_file.read(reinterpret_cast<char*>(&value_count), sizeof(uint64_t));
  values.resize(value_count);
  // read the values
  input_file.read(reinterpret_cast<char*>(values.data()), value_count * sizeof(T));
  input_file.close();

  return values;
}

}  // namespace imib
