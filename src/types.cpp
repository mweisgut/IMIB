#include "types.hpp"

#include <cstdint>
#include <string>

namespace {

using string = std::string;

}  // anonymous namespace

namespace imib {

#define TYPE_SPECIALIZATION(X)        \
  template <>                         \
  const std::string Type<X>::name() { \
    return #X;                        \
  }

TYPE_SPECIALIZATION(int32_t);
TYPE_SPECIALIZATION(int64_t);
TYPE_SPECIALIZATION(uint32_t);
TYPE_SPECIALIZATION(uint64_t);
TYPE_SPECIALIZATION(float);
TYPE_SPECIALIZATION(double);
TYPE_SPECIALIZATION(string);

}  // namespace imib
