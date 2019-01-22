#ifndef INCLUDES_TESTFRAMEWORK_MEMORYLEAKS_HPP_
#define INCLUDES_TESTFRAMEWORK_MEMORYLEAKS_HPP_

#include <SolexOs/memory/SmallHeap.hpp>
#include <gtest/gtest.h>

inline void checkForLeaks() {
  EXPECT_EQ(memory::HEAP<memory::SMALL>::freeSpace(), memory::SMALL_NO);
  EXPECT_EQ(memory::HEAP<memory::MEDIUM>::freeSpace(), memory::MEDIUM_NO);
  EXPECT_EQ(memory::HEAP<memory::LARGE>::freeSpace(), memory::LARGE_NO);
}

#endif
