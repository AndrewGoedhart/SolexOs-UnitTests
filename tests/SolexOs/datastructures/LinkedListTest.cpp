#include <cstdint>
#include <SolexOs/datastructures/LinkedList.hpp>
#include <testFramework/memoryLeaks.hpp>
#include <SolexOs/datastructures/ByteArray.hpp>

#include <gtest/gtest.h>

struct TestObj {
    uint32_t _a;
    uint32_t _b;

    constexpr TestObj() :
        _a(0),
        _b(0) {
    }
    constexpr TestObj(uint32_t a, uint32_t b) :
        _a(a),
        _b(b) {
    }
    constexpr bool operator==(const TestObj &rhs) const {
      return (_a == rhs._a) && (_b == rhs._b);
    }
    constexpr bool operator!=(const TestObj &rhs) {
      return !(*this == rhs);
    }

    template<typename STREAM> inline bool read(STREAM &stream) {
      if( stream.bytesLeft() < 8 ) {
        return false;
      }
      stream.read(_a);
      stream.read(_b);
      return true;
    }

    template<typename STREAM> inline void write(STREAM &stream) const {
      stream.write(_a);
      stream.write(_b);
    }

    size_t streamSize() const {
      return 8;
    }

};

static constexpr TestObj testData[] = {TestObj(1, 2), TestObj(3, 4), TestObj(5, 6), TestObj(7, 8)};

__attribute__ ((optimize("-Og")))
SolexOs::LinkedList<TestObj> createList()
{
  SolexOs::LinkedList<TestObj> list;
  for( uint32_t i = 0; i < 4; i++ ) {
    list.pushBack(testData[i]);
  }
  return list;
}

void verifyList(const SolexOs::LinkedList<TestObj>& list) {
  uint32_t i = 0;
  for( TestObj obj : list ) {
    EXPECT_EQ(obj, testData[i]);
    i++;
  }
  EXPECT_EQ(i, 4U);
}

TEST(LinkedListTest, testPushback) {
  SolexOs::LinkedList<TestObj> list = createList();
  verifyList(list);
}

TEST(LinkedListTest, testErase) {
  SolexOs::LinkedList<TestObj> list = createList();

  for( auto iter = list.begin(); iter != list.end(); ) {
    iter = list.erase(iter);
  }
  for( TestObj &obj : list ) {
    (void)obj;
    FAIL();
  }
  EXPECT_EQ(list.size(), 0u);
}


TEST(LinkedListTest, filterIterator) {
  SolexOs::LinkedList<TestObj> list = createList();

  // single item at end
  for( TestObj obj : list.filter([](const TestObj &obj) {
    return (obj._a==7)&&(obj._b==8);
  }) ) {
    EXPECT_EQ(obj, testData[3]);
  }

  // single item in at start
  for( TestObj obj : list.filter([](const TestObj &obj) {
    return (obj._a==1);
  }) ) {
    EXPECT_EQ(obj, testData[0]);
  }

  // multiple items
  int i = 2;
  for( TestObj obj : list.filter([](const TestObj &obj) {
    return (obj._a>3);
  }) ) {
    EXPECT_LE(i, 4);
    EXPECT_EQ(obj, testData[i]);
    i++;
  }

  // empty case
  for( TestObj obj : list.filter([](const TestObj &obj) {return (obj._a>10);}) ) {
    FAIL();
    EXPECT_EQ(obj._a, 120U);
  }

}

TEST(LinkedListTest, testFind) {
  SolexOs::LinkedList<TestObj> list = createList();

  auto result = list.find([](const TestObj &obj) {
    return (obj._a==7)&&(obj._b==8);
  });

  EXPECT_EQ(*result, testData[3]);

  result = list.find([](const TestObj &obj) {
    return (obj._a==9)&&(obj._b==9);
  });
  EXPECT_EQ(result, nullptr);
}


TEST(LinkedListTest, testSize) {
  EXPECT_EQ(0u, SolexOs::LinkedList<TestObj>().size());

  SolexOs::LinkedList<TestObj> list = createList();

  EXPECT_EQ(4u, list.size());
}


TEST(LinkedListTest, testMemoryAllocation) {
  checkForLeaks();
  {
    auto list = createList();
    EXPECT_EQ(4u, list.size());
  }
  checkForLeaks();

  auto list = createList();
  for( auto iter = list.begin(); iter != list.end(); ) {
    iter = list.erase(iter);
  }
  checkForLeaks();
}

TEST(LinkedListTest, testSerialization) {
  SolexOs::LinkedList<TestObj> list = createList();
  EXPECT_EQ(list.streamSize(), 4U * (4U * 2U) + 1U);

  SolexOs::ByteArray stream(33);

  stream.getWriteStream().write(list);
  SolexOs::LinkedList<TestObj> newList;

  stream.getReadStream().read(newList);

  verifyList(newList);
}

