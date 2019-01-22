#include <gtest/gtest.h>

#include <numeric/FixedPoint.hpp>

using FixedPoint20 = numeric::FixedPoint<20, int32_t>;


TEST(FixedPointTest, Add){
  FixedPoint20 a = FixedPoint20(1,0);
  FixedPoint20 b = FixedPoint20(2,0);
  FixedPoint20 c = FixedPoint20(2,1024*1024-1);
  FixedPoint20 d = FixedPoint20(2,1);
  FixedPoint20 e = FixedPoint20(0,-1);
  FixedPoint20 f = FixedPoint20(0, 1);
  FixedPoint20 g = FixedPoint20(0, 0);
  EXPECT_EQ((a+b).asInt(), 3);
  EXPECT_EQ((c+d).asInt(), 5);
  EXPECT_EQ((e+f).asRaw(), 0);
  EXPECT_EQ((e+g).asRaw(), -1);
}

TEST(FixedPointTest, AddInt){
  FixedPoint20 a = FixedPoint20(1,0);
  EXPECT_EQ((a+1).asRaw(), 2*1024*1024);
  EXPECT_EQ((1+a).asRaw(), 2*1024*1024);
  EXPECT_EQ((0+a).asRaw(), 1*1024*1024);
}

TEST(FixedPointTest, Subtract){
  FixedPoint20 a = FixedPoint20(0,1);
  FixedPoint20 b = FixedPoint20(0,2);
  EXPECT_EQ((a-b).asInt(), -1);
  EXPECT_EQ((a-b).asInt(), -1);
}

TEST(FixedPointTest, SubInt){
  FixedPoint20 a = FixedPoint20(1,0);
  EXPECT_EQ((a-1).asRaw(), 0);
  EXPECT_EQ((1-a).asRaw(), 0);
  EXPECT_EQ((0-a).asInt(), -1);
}


TEST(FixedPointTest, Mult){
  FixedPoint20 a = FixedPoint20(1023,0);
  FixedPoint20 b = FixedPoint20(2,0);
  FixedPoint20 c = FixedPoint20(0,0);
  FixedPoint20 d = FixedPoint20(1,0);
  FixedPoint20 e = FixedPoint20(0,524288);
  EXPECT_EQ((a*b).asInt(), 2046);
  EXPECT_EQ((a*c).asInt(), 0);
  EXPECT_EQ((a*d).asInt(), 1023);
  EXPECT_EQ((b*e).asInt(), 1);
}


TEST(FixedPointTest, MultInt){
  FixedPoint20 a = FixedPoint20(1,0);
  EXPECT_EQ((a*2).asRaw(), 2*1024*1024);
  EXPECT_EQ((2*a).asRaw(), 2*1024*1024);
  EXPECT_EQ((0*a).asInt(), 0);
}

TEST(FixedPointTest, DivInt){
  FixedPoint20 a = FixedPoint20(1,0);
  EXPECT_EQ((a/2).asRaw(), 512*1024);
  EXPECT_EQ((2/a).asRaw(), 2*1024*1024);
  EXPECT_EQ((0/a).asInt(), 0);
}

TEST(FixedPointTest, Div){
  FixedPoint20 a = FixedPoint20(2046,0);
  FixedPoint20 b = FixedPoint20(2,0);
  FixedPoint20 d = FixedPoint20(1,0);
  FixedPoint20 e = FixedPoint20(0,524288);
  FixedPoint20 f = FixedPoint20(0,2047);
  FixedPoint20 g = FixedPoint20(0,2048);
  FixedPoint20 h = FixedPoint20(0,-2048);
  EXPECT_EQ((a/b).asInt(), 1023);
  EXPECT_EQ((a/d).asInt(), 2046);
  EXPECT_EQ((d/e).asInt(), 2);
  EXPECT_EQ((e/b).asRaw(), 262144);
  EXPECT_EQ((f/g).asRaw(), 1048064);
  EXPECT_EQ((h/g).asRaw(), -1024*1024);
}


TEST(FixedPointTest, AsInteger){
  FixedPoint20 a = FixedPoint20(1,0);
  EXPECT_EQ(a.asInt(), 1);

  a = FixedPoint20(2047,0);
  EXPECT_EQ(a.asInt(), 2047);

  a = FixedPoint20(-2048,0);
  EXPECT_EQ(a.asInt(), -2048);

  a = FixedPoint20(0,0);
  EXPECT_EQ(a.asInt(), 0);
}

TEST(FixedPointTest, AsRaw){
  FixedPoint20 a = FixedPoint20(1,0);
  EXPECT_EQ(a.asRaw(), 1<<20);

  a = FixedPoint20(2047,0);
  EXPECT_EQ(a.asRaw(), 2047<<20);

  a = FixedPoint20(-2048,0);
  EXPECT_EQ(a.asRaw(), -2048*1024*1024);

  a = FixedPoint20(0,0);
  EXPECT_EQ(a.asRaw(), 0);

}

TEST(FixedPointTest, Assignment){
  FixedPoint20 a;
  a= 0.5;
  FixedPoint20 b;
  b = 1;
  EXPECT_EQ(a.asRaw(), 512*1024 );
  EXPECT_EQ(b.asRaw(), 1024*1024 );

}

TEST(FixedPointTest, Negation){
  FixedPoint20 a = FixedPoint20(1,0);
  FixedPoint20 b = FixedPoint20(0,0);

  EXPECT_EQ((-a).asRaw(), static_cast<int32_t>(0xFFF00000) );
  EXPECT_EQ((-b).asRaw(), 0x00000000 );
}


TEST(FixedPointTest, Comparison){
  FixedPoint20 a = FixedPoint20(1,0);
  FixedPoint20 b = FixedPoint20(1,1024*1024-1);
  FixedPoint20 c = FixedPoint20(2,0);
  FixedPoint20 d = FixedPoint20(2,0);

  EXPECT_TRUE(b>a );
  EXPECT_TRUE(b>=a );
  EXPECT_TRUE(a<b );
  EXPECT_TRUE(a<=b );

  EXPECT_FALSE(c<d );
  EXPECT_FALSE(c>d );
  EXPECT_TRUE(c==d );
  EXPECT_TRUE(c<=d );
  EXPECT_TRUE(c>=d );

  EXPECT_FALSE(c!=d );
}
