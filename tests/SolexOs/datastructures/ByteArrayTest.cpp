#include <cstdint>
#include <utils/elements.hpp>
#include <SolexOs/datastructures/ByteArray.hpp>
#include <testFramework/memoryLeaks.hpp>

#include <gtest/gtest.h>


using SolexOs::ByteArray;


TEST(ByteArrayTest, staticTest){
  uint8_t data[] = {0x1, 0x2, 0x3, 0x4, 0x5,0x6, 0x7, 0x8};

  ByteArray array(data, elements(data));

  // check nothing was allocated.
  checkForLeaks();
  for( uint32_t i =0; i< elements(data); i++){
    EXPECT_EQ(array[i], i+1);
  }

  // check the right length
  EXPECT_EQ(array.getLength(), 8u);
}

TEST(ByteArrayTest, maxAllocate){
  constexpr auto MAX_ALLOC = memory::LARGE - 1;
  ByteArray array(MAX_ALLOC);

  for( uint32_t i = 0; i < MAX_ALLOC; i++ ) {
    array[i] = static_cast<uint8_t>(i+1);
  }

  for( uint32_t i = 0; i < MAX_ALLOC; i++ ) {
    EXPECT_EQ(array[i], i+1);
  }

  EXPECT_THROW(array[MAX_ALLOC] = 1, std::system_error);
  EXPECT_THROW(ByteArray array(MAX_ALLOC+1), std::system_error);
}

TEST(ByteArrayTest, minAllocate){
  ByteArray array(0);

  EXPECT_THROW(array[0]= 1,  std::system_error);

  array = ByteArray();

  EXPECT_THROW(array[0]= 1,  std::system_error);


  EXPECT_THROW(array.setLength(1),  std::system_error);

  EXPECT_THROW(array.resetLength(),  std::system_error);

}

TEST(ByteArrayTest, testEquals){
  uint8_t data1[] = {0x1, 0x2, 0x3, 0x4, 0x5,0x6, 0x7, 0x8};
  uint8_t data2[] = {0x1, 0x2, 0x3, 0x4, 0x5,0x6, 0x7, 0x7};

  ByteArray array1(data1, elements(data1));
  ByteArray array2(8);

  array2.setBytes(0, data1, 8);
  EXPECT_EQ(array1, array2);

  // try different lengths
  array2.setLength(7);
  EXPECT_FALSE(array1 == array2);

  // try different data
  array2.setLength(8);
  array2.setBytes(0, data2, 8);
  EXPECT_FALSE(array1 == array2);
}


