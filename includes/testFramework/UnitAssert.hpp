#ifndef TESTFRAMEWORK_UNITASSERT_HPP_
#define TESTFRAMEWORK_UNITASSERT_HPP_

#include <gtest/gtest.h>
#include <numeric/Quantity.hpp>
#include <numeric/FixedPoint.hpp>

using numeric::SI::Quantity;

template <typename T, typename X>
void checkTolerance( double tolerance, Quantity<T, X> value, Quantity<T,X> expected){
  double  delta = fabs(value.value.asDouble() - expected.value.asDouble());
  if( delta >= tolerance ){
    std::cout << " " << value.value.asDouble() << " > " << expected.value.asDouble() <<  " tolerance ="<<tolerance <<std::endl;
  }
  EXPECT_LE( delta, tolerance);
}

template <typename T>
void checkTolerance( double tolerance, Quantity<T, double> value, Quantity<T, double> expected){
  double  delta = fabs(value.value - expected.value);
  if( delta >= tolerance ){
    std::cout << " " << value.value << " > " << expected.value <<  " tolerance ="<<tolerance <<std::endl;
  }
  EXPECT_LE( delta, tolerance);
}


inline void checkTolerance( double tolerance, double value, double expected){
  double  delta = fabs(value - expected);
  if( delta >= tolerance ){
    std::cout << " " << value << " > " << expected << " tolerance ="<<tolerance <<std::endl;
  }
  EXPECT_LE( delta, tolerance);
}

template <typename T>
inline void checkTolerance( double tolerance, const T &value, const T &expected){
  double  delta = fabs(value.asDouble() - expected.asDouble());
  if( delta >= tolerance ){
    std::cout << " " << value.asDouble() << " > " << expected.asDouble() << " tolerance ="<<tolerance <<std::endl;
  }
  EXPECT_LE( delta, tolerance);
}

#endif 
