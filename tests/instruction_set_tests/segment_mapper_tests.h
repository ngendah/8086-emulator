#include "segment_mapper.h"

#include "gtest/gtest.h"

TEST(SegmentMapperTests, get_by_default) {
  Registers _registers;
  auto mapper = SegmentMapper(&_registers);
  EXPECT_EQ(&_registers.DS, mapper.get(0, defaults));
}

TEST(SegmentMapperTests, get_by_index) {
  Registers _registers;
  auto mapper = SegmentMapper(&_registers);
  EXPECT_EQ(&_registers.ES, mapper.get(0, indexed));
}
