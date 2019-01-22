#include <SolexOs/memory/SmallHeap.hpp>
#include <gtest/gtest.h>
#include <stdint.h>


template <uint32_t SIZE, uint32_t BLOCKS>
void testHeapAlloc() {
  auto freeSpace = memory::HEAP<SIZE>::freeSpace();
  EXPECT_EQ(freeSpace, BLOCKS);
  uint8_t *block[BLOCKS];
  for( uint32_t i = 0; i < BLOCKS; i++){
    block[i] = memory::allocateSmallBlock(SIZE);
    EXPECT_NE(nullptr, block[i]);
  }
  freeSpace = memory::HEAP<SIZE>::freeSpace();
  EXPECT_EQ(freeSpace, 0u);

  for( uint32_t i = 0; i < BLOCKS*2; i++){
    uint8_t *oldBlock = block[i%BLOCKS];
    memory::freeSmallBlock(block[i%BLOCKS]);
    block[i%BLOCKS] = memory::allocateSmallBlock(SIZE);
    EXPECT_EQ(oldBlock, block[i%BLOCKS]);
  }

  for( uint32_t i = 0; i < BLOCKS; i++){
    memory::freeSmallBlock(block[i]);
  }
  freeSpace = memory::HEAP<SIZE>::freeSpace();
  EXPECT_EQ(BLOCKS, freeSpace);
}


TEST(SmallHeap, AllocAll){
  // memory::resetSmallHeap();
  testHeapAlloc<memory::SMALL, memory::SMALL_NO>();
  testHeapAlloc<memory::MEDIUM, memory::MEDIUM_NO>();
  testHeapAlloc<memory::LARGE, memory::LARGE_NO>();
}


TEST(SmallHeap, OverAllocate){
  memory::resetSmallHeap();
  uint8_t *block[memory::SMALL_NO];
  for( uint32_t i = 0; i < memory::SMALL_NO; i++){
    block[i] = memory::allocateSmallBlock(memory::SMALL);
  }
  EXPECT_THROW(memory::allocateSmallBlock(memory::SMALL), std::system_error);
  for( uint32_t i = 0; i < memory::SMALL_NO; i++){
    memory::freeSmallBlock(block[i]);
  }
  auto freeSpace = memory::HEAP<memory::SMALL>::freeSpace();
  EXPECT_EQ(memory::SMALL_NO, freeSpace);

}

