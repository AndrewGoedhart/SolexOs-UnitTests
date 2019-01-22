#include <gtest/gtest.h>
#include <numeric/Units.hpp>
#include <numeric/Quantity.hpp>

using namespace numeric::SI::FixedPoint;
using numeric::SI::Quantity;

constexpr int32_t FP_ONE = 1024*1024*4;


TEST(UnitQuantitiesTest, testCreateQuantityWVA){
  Watts w = 3.0_W;
  Volts v = 2.0_V;
  Amps expectedI = 1.5_A;
  auto i = w/v;

  EXPECT_EQ( w.value.asRaw(), 3*FP_ONE);
  EXPECT_EQ( expectedI,i);
  EXPECT_EQ( w.value.asRaw(), 3*FP_ONE);


}
using A_per_S = numeric::SI::Unit<0,0,-1,1>;

TEST(UnitQuantitiesTest, testCreateQuantityHV){
  Volts v = 3.0_V;
  Henries h = 2.0_H;
  auto expected = Quantity<A_per_S, FP>(FP(1.5));
  auto i = v/h;
  EXPECT_EQ( expected,i);
}

TEST(UnitQuantitiesTest, testCreateQuantityVAR){
  Volts v = 3.0_V;
  Ohms r= 2.0_R;
  Amps expectedI = 1.5_A;
  Volts expectedV = 3.0_V;
  auto i = v/r;
  EXPECT_EQ( expectedI,i);
  v = i*r;
  EXPECT_EQ( expectedV,v);
}


TEST(UnitQuantitiesTest, testMultNumber){
  Volts v = 3.0_V;
  auto e = v * 2;
  EXPECT_EQ(e.value.asRaw(), 6*FP_ONE );
  e = 2 *v;
  EXPECT_EQ(e.value.asRaw(), 6*FP_ONE );

  e = v * 2.5;
  EXPECT_EQ(e.value.asRaw(), 15*FP_ONE/2 );
  e = 2.5*v;
  EXPECT_EQ(e.value.asRaw(), 15*FP_ONE/2 );

}

TEST(UnitQuantitiesTest, testDivNumber){
  Volts v = 3.0_V;
  auto e = v / 2;
  EXPECT_EQ(e.value.asRaw(), static_cast<int>(1.5*FP_ONE) );
  auto f = 2 / v;
  EXPECT_EQ(f.value.asRaw(), static_cast<int>(2.0/3.0*FP_ONE) );

  e = v / 1.5;
  EXPECT_EQ(e.value.asRaw(), static_cast<int>(2*FP_ONE) );
  f = 4.5/v;
  EXPECT_EQ(f.value.asRaw(), static_cast<int>(1.5*FP_ONE) );

}
