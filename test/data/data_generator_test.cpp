#include "gtest/gtest.h"

#include "data/data_generator.hpp"
#include "types.hpp"

namespace imib {

template <typename TID>
class DataGeneratorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // do nothing
  }

  void TearDown() override {
    // do nothing
  }

  const Data<uint32_t, TID> _expected_data_32_bit_keys{
      std::vector<uint32_t>{286631063, 9501002, 74021799, 149956189, 866868337, 118600844, 356840142, 247178370,
                            54179823, 4952018},
      std::vector<TID>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
      std::vector<std::pair<uint32_t, TID>>{{286631063, 1},
                                            {9501002, 2},
                                            {74021799, 3},
                                            {149956189, 4},
                                            {866868337, 5},
                                            {118600844, 6},
                                            {356840142, 7},
                                            {247178370, 8},
                                            {54179823, 9},
                                            {4952018, 10}},
      std::vector<std::vector<float>>{{286631063.0f},
                                      {9501002.0f},
                                      {74021799.0f},
                                      {149956189.0f},
                                      {866868337.0f},
                                      {118600844.0f},
                                      {356840142.0f},
                                      {247178370.0f},
                                      {54179823.0f},
                                      {4952018.0f}}};
  const Data<uint64_t, TID> _expected_data_64_bit_keys{
      std::vector<uint64_t>{214270672601648144, 7268948875237948928, 2705205453094070272, 1146895480282342016,
                            2754943524098997760, 593666090391845760, 3499648365188541440, 1992215875722076672,
                            2559942653427277824, 658834507063769216},
      std::vector<TID>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
      std::vector<std::pair<uint64_t, TID>>{{214270672601648144, 1},
                                            {7268948875237948928, 2},
                                            {2705205453094070272, 3},
                                            {1146895480282342016, 4},
                                            {2754943524098997760, 5},
                                            {593666090391845760, 6},
                                            {3499648365188541440, 7},
                                            {1992215875722076672, 8},
                                            {2559942653427277824, 9},
                                            {658834507063769216, 10}},
      std::vector<std::vector<float>>{{214270672601648144.0f},
                                      {7268948875237948928.0f},
                                      {2705205453094070272.0f},
                                      {1146895480282342016.0f},
                                      {2754943524098997760.0f},
                                      {593666090391845760.0f},
                                      {3499648365188541440.0f},
                                      {1992215875722076672.0f},
                                      {2559942653427277824.0f},
                                      {658834507063769216.0f}}};
};

using TID_types = ::testing::Types<uint32_t, uint64_t>;
TYPED_TEST_SUITE(DataGeneratorTest, TID_types);

// Test cases are written with the assumption that all competitors support at least the IndexOperationTypes
// InsertEntry, EraseEntry and EqualityLookup

TYPED_TEST(DataGeneratorTest, GenerateTestData32BitKeys) {
  const auto actual_data = DataGenerator::generate<uint32_t, TypeParam>("../resources/test_data/books_10_uint32");
  EXPECT_EQ(actual_data.keys, this->_expected_data_32_bit_keys.keys);
  EXPECT_EQ(actual_data.values, this->_expected_data_32_bit_keys.values);
  EXPECT_EQ(actual_data.entries, this->_expected_data_32_bit_keys.entries);
  EXPECT_EQ(actual_data.keys_float, this->_expected_data_32_bit_keys.keys_float);
}

TYPED_TEST(DataGeneratorTest, GenerateTestData64BitKeys) {
  const auto actual_data = DataGenerator::generate<uint64_t, TypeParam>("../resources/test_data/books_10_uint64");
  EXPECT_EQ(actual_data.keys, this->_expected_data_64_bit_keys.keys);
  EXPECT_EQ(actual_data.values, this->_expected_data_64_bit_keys.values);
  EXPECT_EQ(actual_data.entries, this->_expected_data_64_bit_keys.entries);
  EXPECT_EQ(actual_data.keys_float, this->_expected_data_64_bit_keys.keys_float);
}

}  // namespace imib
